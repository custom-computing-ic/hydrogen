#include <client_api.hpp>
#include <Client.hpp>
#include <message.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

void movingAverage(int n, int size, int* data, int* out) {
  cout << "client - Send job :: movingAverage" << endl;

  int sizeBytes = sizeof(msg_t) + (n + 1) * sizeof(int);
  cout << "Size bytes " << sizeBytes << endl;
  msg_t *msg = (msg_t *)calloc(sizeBytes, 1);

  msg->msgId = MSG_MOVING_AVG;
  msg->dataSize = n;
  msg->paramsSize = 1;
  memcpy(msg->data, data, sizeof(int) * n);
  *(msg->data + sizeof(int) * n) = size;

  for (int i = 0; i < n; i++)
    cout << data[i] << endl;

  const string& name = "localhost";
  int portNumber = 8111;

  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out);
  c.stop();
}
