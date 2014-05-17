#include <message.hpp>

void msg::print() {
  using namespace std;
  cout << "  Id: " << msgId;
  cout << "  Data size: " << dataSize << endl;
  cout << "  Params size: " << paramsSize << endl;
  cout << "  Data: ";
  for (size_t i = 0; i < dataSize; i++) {
    int* i_data = reinterpret_cast<int *>(data);
    cout << i_data[i] << " ";
  }
  cout << endl;
  cout << "  Params: ";
  for (size_t i = dataSize; i < dataSize + paramsSize; i++) {
    int* i_data = reinterpret_cast<int *>(data);
    cout << i_data[i] << " ";
  }

  cout << endl;
}

size_t msg::sizeBytes() {
  return sizeof(msg) + dataBytes();
}

size_t msg::dataBytes() {
  return (dataSize + paramsSize) * sizeof(int);
}

int msg::firstParam() {
  return *(data + dataSize * sizeof(int));
}


msg_t* msg_empty() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  return msg;
}

msg_t* msg_ack() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  msg->msgId = MSG_ACK;
  return msg;
}
