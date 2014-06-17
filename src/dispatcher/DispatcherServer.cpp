#include <DispatcherServer.hpp>
#include <cmath>
#include <vector>
#include <sstream>
#include <boost/chrono.hpp>

using namespace std;
namespace bc = boost::chrono;

void DispatcherServer::movingAverage_cpu(size_t n, size_t size, int *data, int *out) {
  LOGF(debug, "movingAverageCpu(n = %1%, size = %2%)") % n % size;

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
                                         int *data, int *out) {
  LOGF(debug, "movingAverageDfe(n = %1%, size = %2%)") % n % size;
  char* dfeIds[] = {"1", "2", "3", "4"};
#ifdef USEDFE
  MovingAverageDFE(size, n, data, out, 2, dfeIds, true);
#endif
}

void DispatcherServer::movingAverage_dfe(int n, int size,
                                         int *data, int *out,
                                         char** dfeIds, int nDfes) {
  LOGF(debug, "movingAverageDfe(n = %1%, size = %2%, nDfes = %3%)") % n % size % nDfes;
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
					 double *out,
					 int nDFEs
                                         ) {
  LOGF(debug,
       "strike = %1%, sigma = %2%, timestep = %3% "
       "numMaturity = %4%, paraNode = %5% "
       "numPathGroup = %6%, T = %7%, nDfes = %8%")
    % strike % sigma % timestep % numMaturity % paraNode % numPathGroup % T % nDFEs;

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
		       nDFEs);
#endif
}

msg_t* DispatcherServer::handle_request(msg_t* request) {
  LOG(debug) << "Start";

  if (request->msgId == MSG_MOVING_AVG) {
    // unpack data
    auto start = bc::system_clock::now();
    size_t n = request->dataSize;
    size_t nBytes = sizeof(int) * n;
    int* out = (int *)calloc(n, sizeof(int));
    int* data_in = (int*)malloc(nBytes);
    LOG(debug) << "(DEBUG): RIDS[";
    std::vector<char*> dfeIds;
    std::stringstream ss;
    int nDFEs = 0;
    for (int i = 0; i < 4; i++) {
      if ((int)(pow(2,i)) & request->rids) {
        char* temp = (char*) malloc (sizeof(char));
        LOG(debug) << 1;
        ss << i;
        ss >> *temp;
        dfeIds.push_back(temp);
        nDFEs++;
      } else {
        LOG(debug) << 0;
      }
    }
    LOG(debug) << "]\n";
    memcpy(data_in, request->data, nBytes);

    // do computation
    // TODO check resource type
    if (request->resourceType == DFE && useDfe) {
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

    // write the response
    int dataBytes = request->dataSize * sizeof(int);
    size_t sizeBytes = sizeof(msg_t) + dataBytes;
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = n;
    response->paramsSize = 0;
    response->totalBytes = sizeBytes;
    response->dataBytes = dataBytes;
    response->predicted_time = bc::duration_cast<bc::milliseconds>(end-start).count();
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

    double res;

    // TODO add nDFEs
    int nDFEs = 1;

    this->optionPricing_dfe(strike,
			    sigma,
			    timestep,
			    numMaturity,
			    paraNode,
			    numPathGroup,
			    T,
			    &res,
			    nDFEs);

    // write the response
    size_t sizeBytes = sizeof(msg_t) + sizeof(double);
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = 1;
    response->paramsSize = 0;
    response->totalBytes = sizeBytes;
    response->dataBytes = sizeof(double);
    memcpy(response->data, &res, sizeof(double));

    return response;
  }

  return msg_empty();
}
