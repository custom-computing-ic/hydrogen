#include <Server.hpp>

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

void Server::start() {
  cout << "Starting Server... " << endl;
  cout << "Name:     " << name << endl;
  cout << "Port:     " << portNumber << endl;
  char hn[1024];
  gethostname(hn, 1024);
  cout << "Hostname: " << hn << endl;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  cout << "Sockfd:   " << sockfd << endl;

  if (sockfd < 0)
    cout << "ERROR opening socket" << endl;

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0)
    cout << "ERROR on binding" << endl;

  listen(sockfd, 5);
  mainServerLoop();

  // cleanup stuff
  close(sockfd);
}

void Server::mainServerLoop() {
  while (!shuttingDown) {
    newsockfd = accept(sockfd, NULL, NULL);

    if (shuttingDown) {
      close(newsockfd);
      break;
    }

    if (newsockfd < 0) {
      cout << "ERROR on accept";
      break;
    }

    // TOOD fork here to handle the request
    msg_t* msg = NULL;
    do {
      int maxLen = 2048;
      char buffer[maxLen];
      bzero(buffer, maxLen);
      int n = read(newsockfd, buffer, maxLen);
      if (n < 0) {
        cout << "ERROR reading from socket";
        continue;
      }

      // rebuild msg
      msg = (msg_t*)buffer;
      cout << "Received message:" << endl;
      cout << "  Id: " << msg->msgId;
      cout << "  Data size: " << msg->dataSize << endl;
      cout << "  Data: ";
      for (int i = 0; i < msg->dataSize; i++) {
        cout << *(((int *)msg->data) + i) << " ";
      }
      cout << endl;

      n = write(newsockfd,"Ack", 3);
      if (n < 0)
        cout << "ERROR writing to socket" << endl;

      handleRequest(*msg);
    } while (msg!= NULL && msg->msgId != MSG_DONE && !shuttingDown);
    cout << "Closing connection " << newsockfd << endl;
    close(newsockfd);
  }
}

void Server::stop() {
  cout << "Server::shutting down..." << endl;
  shuttingDown = true;
  close(newsockfd);
  close(sockfd);
}
