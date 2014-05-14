#include <Server.hpp>

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
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
  int children = 0;
  int maxChildren = 0;
  while (!shuttingDown) {
    while (children > maxChildren) {
      wait(NULL);
      children--;
    }
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
    int pid = fork();

    if (pid > 0) {
      children++;
      continue;
    } else if (pid < 0) {
      cout << "Error forking process" << endl;
    } else {
      // in child process
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
        msg->print();

        n = write(newsockfd,"Ack", 3);
        if (n < 0)
          cout << "ERROR writing to socket" << endl;

        int sizeBytes = sizeof(msg_t) + (msg->dataSize) * sizeof(int);
        cout << sizeBytes << endl;
        msg_t* response = (msg_t *)malloc(sizeBytes);

        cout << "Handling request " << endl;
        handleRequest(*msg, *response);

        cout << "Handled request. Sending response: " << endl;
        response->print();
        n = send(newsockfd, response,  sizeBytes, 0);
        free(response);
      } while (msg != NULL && msg->msgId != MSG_DONE && !shuttingDown);
      cout << "Closing connection " << newsockfd << endl;
      close(newsockfd);
      exit(0);
    }
  }

  // make sure all child processes have finished
  while (children > 0) {
    wait(NULL);
    children--;
  }

}

void Server::stop() {
  cout << "Server::shutting down..." << endl;
  shuttingDown = true;
  close(newsockfd);
  close(sockfd);
}
