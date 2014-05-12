#include <Server.hpp>

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

void Server::start() {
  cout << "Starting Server " << endl;
  cout << "Name:   " << name << endl;
  cout << "Port:   " << portNumber << endl;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    cerr << "ERROR opening socket" << endl;

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0)
    cerr << "ERROR on binding" << endl;

  listen(sockfd, 5);
  mainServerLoop(sockfd);

  // cleanup stuff
  close(sockfd);
}

void Server::mainServerLoop(int sockfd) {
  while (true) {
    int newsockfd = accept(sockfd, NULL, NULL);

    if (newsockfd < 0)
      cerr << "ERROR on accept";

    int maxLen = 2048;
    char buffer[maxLen];
    bzero(buffer, maxLen);
    int n = read(newsockfd, buffer, maxLen);
    if (n < 0)
      cerr << "ERROR reading from socket";

    // rebuild msg
    msg_t* msg = (msg_t*)buffer;

    cout << "Received message:" << endl;
    cout << "Id: " << msg->msgId << endl;
    cout << "Data size: " << msg->dataSize << endl;

    for (int i = 0; i < msg->dataSize; i++) {
      cout << *(((int *)msg->data) + i) << endl;
    }

    if (n < 0)
      cerr << "ERROR writing to socket";

    handleRequest(*msg);

    close(newsockfd);
  }
}
