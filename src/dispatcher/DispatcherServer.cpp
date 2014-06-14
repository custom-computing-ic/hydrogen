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
}
void DispatcherServer::movingAverage_dfe(int n, int size,
                                         int *data, int *out) {
  cout << "Dispatcher::MovingAverageDFE" << endl;
  char* dfeIds[] = {"1", "2", "3", "4"};
#ifdef USEDFE
  MovingAverageDFE(size, n, data, out, 2, dfeIds, true);
#endif
}

void DispatcherServer::movingAverage_dfe(int n, int size,
                                         int *data, int *out,
                                         char** dfeIds, int nDfes) {
  cout << "Dispatcher::MovingAverageDFE" << endl;
  //  char* dfeIds[] = {"1", "2", "3", "4"};
#ifdef USEDFE
  MovingAverageDFE(size, n, data, out, nDfes, dfeIds, true);
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
#ifdef USEDFE
  *out = optionPricing(strike,
		       sigma,
		       timestep,
		       numMaturity,
		       paraNode,
		       numPathGroup,
		       T,
		       NULL,
		       NULL);
#endif
}

msg_t* DispatcherServer::handle_request(msg_t* request) {
#ifdef DEBUG
  std::cout << "Dispatcher::handle_request" << std::endl;
#endif

  if (request->msgId == MSG_MOVING_AVG) {
    // unpack data
    auto start = bc::system_clock::now();
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
    // TODO check resource type
    if (/*request->resourceType == "DFE" &&*/ useDfe) {
      // TODO pass in other arguments (e.g. nDfes, dfeIDs)
      if (nDFEs != 0)
        movingAverage_dfe(n, request->firstParam(), data_in, out, dfeIds.data(),nDFEs);
      else
        movingAverage_dfe(n,request->firstParam(), data_in, out);
    } else {
      movingAverage_cpu(n, (size_t)request->firstParam(), data_in, out);
    }
    for (auto c : dfeIds) {
      free(c);
    }
    auto end = bc::system_clock::now();
#ifdef DEBUG
    cout << "Data out " << endl;
    for (int i = 0; i < n; i++)
      cout << out[i] << endl;
#endif

    // write the response
    size_t sizeBytes = sizeof(msg_t) + request->dataSize * sizeof(int);
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = n;
    response->paramsSize = 0;
    response->predicted_time = bc::duration_cast<bc::milliseconds>(end-start).count();
    memcpy(response->data, out, nBytes);
    free(out);
    free(data_in);

    return response;
  } else if (request->msgId == MSG_OPTION_PRICE) {

    // -- Dummy Parameters --
    double strike = 0.01;
    double sigma = 0.02;
    double timestep = 0.05;
    int numTimeStep = (int)(10/0.05);
    int numMaturity = 2000000;
    int paraNode = 50;
    int numPathGroup = 20;
    int numPE = 4;
    double T = 10;

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
    memcpy(response->data, &res, sizeof(double));
    return response;
  }

  return msg_empty();
}
