#include "client.hpp"

#include <iostream>
#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

#pragma pack(1)
typedef struct msg {
  int msgId; // based on msg id we know what the data means
  int dataSize;
  char data[];
} msg_t;
#pragma pack(0)

static void error(char *err) {
  cerr << err << endl;
}

static void send (msg_t *message, int sizeBytes) {
  struct sockaddr_in serv_addr;

  int portno = 8111;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
    error("ERROR opening socket");
  struct hostent* server = gethostbyname("localhost");

  if (server == NULL) {
    error("ERROR, no such host\n");
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  int n = send(sockfd, message, sizeBytes, 0);

  if (n < 0)
    error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0)
    error("ERROR reading from socket");
  printf("%s\n",buffer);
  close(sockfd);
}

void movingAverage(int n, int* data) {
  using namespace std;
  cout << "client - Send job :: movingAverage" << endl;

  int sizeBytes = sizeof(msg_t) + n * sizeof(int);
  cout << "Size bytes " << sizeBytes << endl;
  msg_t *msg = (msg_t *)malloc(sizeBytes);

  msg->msgId = 0;
  msg->dataSize = n;
  memcpy(msg->data, data, sizeof(int) * n);

  for (int i = 0; i < n; i++)
    cout << data[i] << endl;

  send(msg, sizeBytes);
}
