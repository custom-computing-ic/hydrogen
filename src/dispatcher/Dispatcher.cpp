#include <Dispatcher.hpp>
#include <message.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

void Dispatcher::movingAverage(int n, int size, int *data, int *out) {
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

void Dispatcher::handleRequest(msg_t& message, msg_t& response) {
  cout << "Handling request " << endl;

  if (message.msgId == MSG_MOVING_AVG) {

    // unpack data
    int n = message.dataSize;
    int nBytes =sizeof(int) * n;
    int out[n];
    int* data_in = (int*)malloc(nBytes);
    memcpy(data_in, message.data, nBytes);

    cout << "Data in " << endl;
    for (int i = 0; i < n; i++)
      cout << data_in[i] << endl;

    // do computation
    movingAverage(n, message.firstParam(), data_in, out);

    cout << "Data out " << endl;
    for (int i = 0; i < n; i++)
      cout << out[i] << endl;

    // write the response
    response.msgId = MSG_RESULT;
    response.dataSize = n;
    response.paramsSize = 0;

    memcpy(response.data, out, nBytes);
  }
}
