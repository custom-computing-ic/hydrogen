#include <client_api.hpp>
#include <Client.hpp>
#include <message.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

void movingAverage(size_t n, size_t size, int* data, int* out) {
  cout << "client - Send job :: movingAverage" << endl;

  size_t sizeBytes = sizeof(msg_t) + (n + 1) * sizeof(int);
  msg_t *msg = (msg_t *)calloc(sizeBytes, 1);

  msg->msgId = MSG_MOVING_AVG;
  msg->dataSize = n;
  msg->paramsSize = 1;
  memcpy(msg->data, data, sizeof(int) * n);
  memcpy(msg->data + sizeof(int) * n, (char *)size, sizeof(int));

  const string& name = "localhost";
  int portNumber = 8111;

  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out);
  c.stop();

  free(msg);
}
