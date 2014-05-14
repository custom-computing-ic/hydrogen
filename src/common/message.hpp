#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <iostream>

/** Message ids */
const int MSG_DONE = 0;
const int MSG_ACK = 1;
const int MSG_MOVING_AVG = 2;
const int MSG_RESULT = 100;

#pragma pack(1)
typedef struct msg {
  int msgId; // based on msg id we know what the data means
  int dataSize;
  int paramsSize;
  char data[];

  int sizeBytes() {
    return sizeof(msg) + dataBytes();
  }

  int dataBytes() {
    return (dataSize + paramsSize) * sizeof(int);
  }

  int firstParam() {
    return *(data + dataSize * sizeof(int));
  }

  void print() {
    using namespace std;
    cout << "  Id: " << msgId;
    cout << "  Data size: " << dataSize << endl;
    cout << "  Params size: " << paramsSize << endl;
    cout << "  Data: ";
    for (int i = 0; i < dataSize; i++) {
      cout << *(((int *)data) + i) << " ";
    }
    cout << endl;
    cout << "  Params: ";
    for (int i = dataSize; i < dataSize + paramsSize; i++) {
      cout << *(((int *)data) + i) << " ";
    }
    cout << endl;
  }

} msg_t;
#pragma pack(0)

#endif /* _MESSAGE_H_ */
