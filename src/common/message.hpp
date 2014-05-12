#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#pragma pack(1)
typedef struct msg {
  int msgId; // based on msg id we know what the data means
  int dataSize;
  char data[];
} msg_t;
#pragma pack(0)

#endif /* _MESSAGE_H_ */
