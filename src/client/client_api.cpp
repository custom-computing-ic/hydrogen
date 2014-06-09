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
  int client_id = 0;
  int portNumber = 8111;

  char* clientIdCh = getenv("CLIENT_ID");
  if (clientIdCh == NULL) {
    cout << "(ERROR): ClientAPI Call- Environmental Variable CLIENT_ID"
         << " is not set!\n(ERROR): Using default port 8111\n";
  } else {
    client_id = atoi(clientIdCh);
    cout << "(DEBUG): CLIENT_ID= " << clientIdCh << "    Connecting to " 
         << client_id + 8110 << "\n";
  }
  /* hack to make the default port 8111 and client ports 8112-8120 */
  portNumber += client_id > 0 ? client_id -1 : client_id;
  msg->clientId = client_id;
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


/*Control messages.... needs protobuf commenting out for now
 * void addResource(std::string type, std::string host, std::string port, int clientId) {
  cout << "Client :: addResource" << endl;
  const string& name = "localhost";
  int portNumber = 8110 + clientId;
  cout << "\tConnecting to " << name << ":" << portNumber << endl;

  Client c(portNumber,name);
  c.start();
  msg_t* msg = msg_control_add_res(type,host,port);

    
  msg->clientId = clientId;
  c.send(msg);
  c.stop();
//  free(msg);
//  free(buff);
}*/
