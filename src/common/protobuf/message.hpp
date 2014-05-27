#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <iostream>
#include <cstdlib>
#include <packet.pb.h>
/** Message ids */
const int MSG_DONE = 0;
const int MSG_ACK = 1;
const int MSG_MOVING_AVG = 2;
const int MSG_RESULT = 100;
const int MSG_CONTROL = 200;
#pragma pack(1)
typedef struct msg {
  int msgId; // based on msg id we know what the data means
  int clientId;  //generated by executor - used for priority scheduling

  size_t dataSize;
  size_t paramsSize;

  size_t sizeBytes();
  size_t dataBytes();
  int firstParam();
  void print();
  void print_control();


  char data[];

  } msg_t;
#pragma pack(0)

/** Utility functions for constructing various types of messages */
msg_t* msg_empty();
msg_t* msg_ack();
msg_t* msg_moving_avg(int n, size_t width, int* dataIn);
msg_t* msg_control_add_res(std::string type, std::string host, std::string port);
#endif /* _MESSAGE_H_ */
