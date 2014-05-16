#include <DispatcherServer.hpp>

using namespace std;

void DispatcherServer::movingAverage(int n, int size, int *data, int *out) {
  cout << "Dispatcher::MovingAverage" << endl;
  cout << " n:    " << n << endl;
  cout << " size: " << size << endl;

  for (int i = 0; i <= n - size; i++) {
    out[i] = 0;
    for (int j = 0; j < size; j++)
      out[i] += data[i + j];
    out[i] /= size;
  }

  for (int i = n - size + 1; i < n; i++) {
    out[i] = 0;
  }
}

msg_t* DispatcherServer::handle_request(msg_t* request) {
  std::cout << "Dispatcher:: do work" << std::endl;

  if (request->msgId == MSG_MOVING_AVG) {
    // unpack data
    int n = request->dataSize;
    int nBytes =sizeof(int) * n;
    int out[n];
    int* data_in = (int*)malloc(nBytes);
    memcpy(data_in, request->data, nBytes);

    cout << "Data in " << endl;
    for (int i = 0; i < n; i++)
      cout << data_in[i] << endl;

    // do computation
    movingAverage(n, request->firstParam(), data_in, out);

    cout << "Data out " << endl;
    for (int i = 0; i < n; i++)
      cout << out[i] << endl;

    // write the response
    int sizeBytes = sizeof(msg_t) + request->dataSize * sizeof(int);
    msg_t* response = (msg_t*)calloc(sizeBytes, 1);
    response->msgId = MSG_RESULT;
    response->dataSize = n;
    response->paramsSize = 0;
    memcpy(response->data, out, nBytes);
    return response;
  }

  return msg_empty();
}
