#include <DispatcherServer.hpp>
#include <cmath>
#include <vector>
#include <sstream>
#include <boost/chrono.hpp>
using namespace std;
namespace bc = boost::chrono;

void DispatcherServer::movingAverage_cpu(size_t n, size_t size, int *data, int *out) {
  cout << "Dispatcher::MovingAverageCPU" << endl;
  cout << " n:    " << n << endl;
  cout << " size: " << size << endl;

  for (size_t i = 0; i <= n - size; i++) {
    out[i] = 0;
    for (size_t j = 0; j < size; j++)
      out[i] += data[i + j];
    out[i] /= size;
  }

  for (size_t i = n - size + 1; i < n; i++) {
    out[i] = 0;
  }
  /* [TODO]: remove this hack to make DFE & CPU implmentations
   * the same...
   */
  std::rotate(&out[0],&out[n-1],&out[n]);
}

void DispatcherServer::movingAverage_dfe(int n, int size,
                                         int *data, int *out,
                                         bool useGroups) {
  cout << "Dispatcher::MovingAverageDFE" << endl;
  char* dfeIds[] = {"1", "2", "3", "4"};
//  int nDfes = 4;
#ifdef USEDFE
  MovingAverageDFE(size, n, data, out, this->nDfes, dfeIds, useGroups);
#endif
}

void DispatcherServer::movingAverage_dfe(int n, int size,
                                         int *data, int *out,
                                         char** dfeIds, int nDfes,
                                         bool useGroups) {
  cout << "Dispatcher::MovingAverageDFE" << endl;
  //  char* dfeIds[] = {"1", "2", "3", "4"};
#ifdef USEDFE
  MovingAverageDFE(size, n, data, out, nDfes, dfeIds, useGroups);
#endif
}

void DispatcherServer::optionPricing_dfe(double strike,
                                         double sigma,
                                         double timestep,
                                         int numMaturity,
                                         int paraNode,
                                         int numPathGroup,
                                         double T,
					 double *out
                                         ) {
  std::cout << "sigma: " << sigma << "\ttimestep: " << timestep
            << "\t\tnumMaturity: " << numMaturity << "\n";
  std::cout << "paraNode: " << paraNode << "\tnumPathGroup: " << numPathGroup
            << "\tT: " << T << "\n";
  int nDfes = 1;
#ifdef USEDFE
  *out = optionPricing(strike,
		       sigma,
		       timestep,
		       numMaturity,
		       paraNode,
		       numPathGroup,
		       T,
		       NULL,
		       NULL,
           nDfes);
#endif
}

msg_t* DispatcherServer::handle_request(msg_t* request) {
#ifdef DEBUG
  std::cout << "Dispatcher::handle_request" << std::endl;
#endif

  if (request->msgId == MSG_MOVING_AVG) {
    // unpack data

    size_t n = request->dataSize;
    size_t nBytes = sizeof(int) * n;
    int* out = (int *)calloc(n, sizeof(int));
    int* data_in = (int*)malloc(nBytes);
    std::cout << "(DEBUG): RIDS[";
    std::vector<char*> dfeIds;
    std::stringstream ss;
    int nDFEs = 0;
    for (int i = 0; i < 4; i++) {
      if ((int)(pow(2,i)) & request->rids) {
        char* temp = (char*) malloc (sizeof(char));
        std::cout << 1;
        ss << i;
        ss >> *temp;
        dfeIds.push_back(temp);
        nDFEs++;
      } else {
        std::cout << 0;
      }
    }
    std::cout << "]\n";
    memcpy(data_in, request->data, nBytes);

    // do computation
    auto start = bc::system_clock::now();
    if (request->resourceType == DFE && useDfe) {
      // TODO pass in other arguments (e.g. nDfes, dfeIDs)
      if (nDFEs != 0)
        movingAverage_dfe(n, request->firstParam(), data_in, out, dfeIds.data(),nDFEs,
                          useGroups);
      else
        movingAverage_dfe(n,request->firstParam(), data_in, out,useGroups);
    } else {
      movingAverage_cpu(n, (size_t)request->firstParam(), data_in, out);
    }
    auto end = bc::system_clock::now();
    for (auto c : dfeIds) {
      free(c);
    }
    auto busy_time = bc::duration_cast<bc::milliseconds>(end-start).count();

#ifdef DEBUG
    cout << "Data out " << endl;
    for (int i = 0; i < n; i++)
      cout << out[i] << endl;
#endif

    // write the response
    int dataBytes = request->dataSize * sizeof(int);
    size_t sizeBytes = sizeof(msg_t) + dataBytes;
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = n;
    response->paramsSize = 0;
    response->totalBytes = sizeBytes;
    response->dataBytes = dataBytes;
    response->predicted_time = nDFEs > 0 ? busy_time*nDFEs : busy_time;
    memcpy(response->data, out, nBytes);
    free(out);
    free(data_in);

    return response;
  } else if (request->msgId == MSG_OPTION_PRICE) {

    // -- Dummy Parameters --
    double strike = *(double*)request->data;
    double sigma = *(double*)(request->data + sizeof(double));
    double timestep = *(double*)(request->data + 2* sizeof(double));
    int numTimeStep = (int)(10/0.05);
    int numMaturity = *(int*)(request->data + 3*sizeof(double));
    int paraNode = *(int*)(request->data + 3*sizeof(double) + sizeof(int));
    int numPathGroup = *(int*)(request->data + 3*sizeof(double) + 2*sizeof(int));
    int numPE = 4;
    double T = *(double*)(request->data + 3*sizeof(double) + 3*sizeof(int));

    cerr << "Making request " << endl;
    double res;

    this->optionPricing_dfe(strike,
			    sigma,
			    timestep,
			    numMaturity,
			    paraNode,
			    numPathGroup,
			    T,
			    &res);

    // write the response
    size_t sizeBytes = sizeof(msg_t) + sizeof(double);
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = 1;
    response->paramsSize = 0;
    response->totalBytes = sizeBytes;
    response->dataBytes = sizeof(double);
    memcpy(response->data, &res, sizeof(double));

    cerr << "Got reply " << endl;
    response->print();
    return response;
  }

  return msg_empty();
}
