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

  msg_t *msg = msg_moving_avg(n, size, data);

  const string& name = "localhost";
  int portNumber = 8111;

  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out, n * sizeof(int));
  c.stop();

  free(msg);
}


void movingAverage(size_t n, size_t size, int* data, int* out, int pNum) {
  cout << "client - Send job :: movingAverage" << endl;

  msg_t *msg = msg_moving_avg(n, size, data);

  const string& name = "localhost";
  int portNumber = 8110 + pNum;
  msg->clientId = pNum;
  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out, n * sizeof(int));
  c.stop();

  free(msg);
}


void addResource(std::string type, std::string host, std::string port, int clientId) {
  cout << "client - Send Control :: addResource" << endl;
  msg_t* msg = msg_control_add_res(type,host,port);
  const string& name = "localhost";
  int portNumber = 8110 + clientId;
  msg->clientId = clientId;

  Client c(portNumber,name);
  c.start();
  c.send(msg);
  char buf[10];
  c.getResult(buf, 1);
  c.stop();
//  free(msg);
}
