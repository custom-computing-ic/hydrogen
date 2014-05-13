#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/** Message ids */
const int MSG_DONE = 0;
const int MSG_ACK = 1;
const int MSG_MOVING_AVG = 2;

#pragma pack(1)
typedef struct msg {
  int msgId; // based on msg id we know what the data means
  int dataSize;
  int paramsSize;
  char data[];

  int sizeBytes() {
    return sizeof(msg) + (dataSize + paramsSize) * sizeof(int);
  }

} msg_t;
#pragma pack(0)

#endif /* _MESSAGE_H_ */
