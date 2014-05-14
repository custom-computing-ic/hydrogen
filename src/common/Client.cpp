#include <Client.hpp>

#include <iostream>
/* For read() */
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

static void error(string err) {
  cout << err << endl;
}

void Client::send (msg_t *message, int sizeBytes) {
  cout << "Sending " << sizeBytes << " to " << sockfd << endl;
  int n = ::send(sockfd, message, sizeBytes, 0);
  if (n < 0)
    error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer, 256);
  cout << "Waiting for reply... ";
  n = read(buffer, 255);
  buffer[3] = '\0';
  if (n < 0)
    cout << "ERROR reading from socket" << endl;
  cout << string(buffer) << endl;
}

void Client::start() {
  cout << "Starting Client..." << endl;
  cout << "Hostname: " << name << endl;
  cout << "Port: " << portNumber << endl;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  cout << "Sockfd: " << sockfd << endl;

  struct sockaddr_in serv_addr;

  if (sockfd < 0)
    error("ERROR opening socket");
  struct hostent* server = gethostbyname(name.c_str());

  if (server == NULL) {
    error("ERROR, no such host\n");
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portNumber);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");
}

void Client::stop() {
  cout << "Client::shutting down..." << endl;

  // let the server know we're done
  msg_t msg;
  msg.msgId = MSG_DONE;
  msg.dataSize = 0;
  msg.paramsSize = 0;
  send(&msg, sizeof(msg_t));

  ::close(sockfd);
}

void Client::getResult(void* out) {
  // wait for reply back
  // XXX determine buffer size dynamically
  char buffer[1024];
  bzero(buffer, 1024);
  int n = read(Client::sockfd, buffer, 1024);

  msg_t* rsp = (msg_t*)buffer;
  memcpy(out, rsp->data, rsp->dataBytes());
}
