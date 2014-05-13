#include <client_api.hpp>
#include <Client.hpp>
#include <message.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

void movingAverage(int n, int* data) {
  cout << "client - Send job :: movingAverage" << endl;

  int sizeBytes = sizeof(msg_t) + n * sizeof(int);
  cout << "Size bytes " << sizeBytes << endl;
  msg_t *msg = (msg_t *)malloc(sizeBytes);

  msg->msgId = 0;
  msg->dataSize = n;
  memcpy(msg->data, data, sizeof(int) * n);

  for (int i = 0; i < n; i++)
    cout << data[i] << endl;

  const string& name = "localhost";
  int portNumber = 8111;

  Client c(portNumber, name);
  c.start();
  c.send(msg, sizeBytes);
  c.close();
}
