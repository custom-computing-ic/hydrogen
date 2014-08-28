#include <client_api.hpp>
#include <Client.hpp>
#include <message.hpp>

#include <boost/current_function.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <Logging.hpp>

using namespace std;

void movingAverage(size_t n, size_t size, int* data, int* out) {
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
    LOG(debug) << "(DEBUG):\t- CLIENT_ID= " << clientIdCh << "\tConnecting to "
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

void optionPricing(double strike,
		   double sigma,
		   double timestep,
		   int numMaturity,
		   int paraNode,
		   int numPathGroup,
		   double T,
		   double *out) {
  LOG(debug) << "Send optionPricing request ";

  msg_t *msg = msg_option_pricing(strike,
				  sigma,
				  timestep,
				  numMaturity,
				  paraNode,
				  numPathGroup,
				  T);

  const string& name = "localhost";
  int client_id = 0;
  int portNumber = 8111;

  char* clientIdCh = getenv("CLIENT_ID");
  if (clientIdCh == NULL) {
    cout << "(ERROR): ClientAPI Call- Environmental Variable CLIENT_ID"
         << " is not set!\n(ERROR): Using default port 8111\n";
  } else {
    client_id = atoi(clientIdCh);
    LOG(debug) << "\t- CLIENT_ID= " << clientIdCh << "\tConnecting to "
         << client_id + 8110 << "\n";
  }
  /* hack to make the default port 8111 and client ports 8112-8120 */
  portNumber += client_id > 0 ? client_id -1 : client_id;
  msg->clientId = client_id;

  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out, sizeof(double));
  c.stop();

  free(msg);
}

/** For running without executor and with predicted and target
    execution times */
void optionPricingJlo(double strike,
		      double sigma,
		      double timestep,
		      int numMaturity,
		      int paraNode,
		      int numPathGroup,
		      double T,
		      double *out,
		      int predictedTimeMillis,
		      int targetExecutionTimeMillis) {
  LOG(debug) << "Send optionPricing request ";

  msg_t *msg = msg_option_pricing(strike,
				  sigma,
				  timestep,
				  numMaturity,
				  paraNode,
				  numPathGroup,
				  T);

  const string& name = "localhost";
  int client_id = 0;
  int portNumber = 8111;

  char* clientIdCh = getenv("CLIENT_ID");
  if (clientIdCh == NULL) {
    cout << "(ERROR): ClientAPI Call- Environmental Variable CLIENT_ID"
         << " is not set!\n(ERROR): Using default port 8111\n";
  } else {
    client_id = atoi(clientIdCh);
    LOG(debug) << "\t- CLIENT_ID= " << clientIdCh << "\tConnecting to "
         << client_id + 8110 << "\n";
  }
  /* hack to make the default port 8111 and client ports 8112-8120 */
  portNumber += client_id > 0 ? client_id -1 : client_id;
  msg->clientId = client_id;

  msg->priority = 1;
  msg->avg_wt = 3;
  msg->predictedTimeMillis = predictedTimeMillis;
  msg->resourceType = DFE;
  msg->targetExecutionTimeMillis = targetExecutionTimeMillis;

  Client c(portNumber, name);
  c.start();
  c.send(msg);
  c.getResult(out, sizeof(double));
  c.stop();

  free(msg);
}

void TerminateServer() {
  LOG(debug) << "Start";

  msg_t *msg = msg_empty();

  const string& name = "localhost";
  int client_id = 0;
  int portNumber = 8111;

  char* clientIdCh = getenv("CLIENT_ID");
  if (clientIdCh == NULL) {
    cout << "(ERROR): ClientAPI Call- Environmental Variable CLIENT_ID"
         << " is not set!\n(ERROR): Using default port 8111\n";
  } else {
    client_id = atoi(clientIdCh);
    cout << "(DEBUG):\t- CLIENT_ID= " << clientIdCh << "\tConnecting to "
         << client_id + 8110 << "\n";
  }
  /* hack to make the default port 8111 and client ports 8112-8120 */
  portNumber += client_id > 0 ? client_id -1 : client_id;
  msg->clientId = client_id;
  msg->msgId = MSG_TERM;
  char buff[1024];
  Client c(portNumber, name);
  c.start();
  c.send(msg);
  free(msg);
}

void movingAverage(size_t n, size_t size, int* data, int* out, int pNum) {
  cout << "(DEBUG): Send job :: movingAverage, Priority: "
       << pNum << endl;

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
