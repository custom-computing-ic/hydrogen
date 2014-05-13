#include <Client.hpp>

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

static void error(string err) {
  cerr << err << endl;
}

void Client::send (msg_t *message, int sizeBytes) {
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

  int n = ::send(sockfd, message, sizeBytes, 0);

  if (n < 0)
    error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0)
    error("ERROR reading from socket");
  cout << buffer << endl;

}

void Client::start() {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
}

void Client::close() {
  ::close(sockfd);
}
