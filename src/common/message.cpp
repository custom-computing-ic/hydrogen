#include <message.hpp>
#include <cstring>

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
msg_t* msg_done() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  msg->msgId = MSG_DONE;
  return msg;
}

msg_t* msg_moving_avg(int n, size_t width, int* dataIn) {
  int sizeBytes = sizeof(msg_t) + n * sizeof(int) + sizeof(size_t);
  msg_t* msg = (msg_t *)calloc(sizeBytes, 1);
  msg->msgId = MSG_MOVING_AVG;
  msg->dataSize = n;
  msg->paramsSize = 1;
  memcpy(msg->data, dataIn, sizeof(int) * n);
  memcpy(msg->data + sizeof(int) * n, (char *)&width, sizeof(size_t));
  return msg;
}

msg_t* msg_option_pricing(double strike,
			  double sigma,
			  double timestep,
			  int numMaturity,
			  int paraNode,
			  int numPathGroup,
			  double T) {
  int sizeBytes = sizeof(msg_t) + 4 * sizeof(double) + 3 * sizeof(int);
  msg_t* msg = (msg_t *)calloc(sizeBytes, 1);
  msg->msgId = MSG_OPTION_PRICE;
  msg->dataSize = 0;
  msg->paramsSize = 7;

  // XXX find a better way for this...
  memcpy(msg->data, &strike, sizeof(double));
  memcpy(msg->data + sizeof(double), &sigma, sizeof(double));
  memcpy(msg->data + 2 * sizeof(double), &timestep, sizeof(double));
  memcpy(msg->data + 3 * sizeof(double), &numMaturity, sizeof(int));
  memcpy(msg->data + 3 * sizeof(double) + sizeof(int), &paraNode, sizeof(int));
  memcpy(msg->data + 3 * sizeof(double) + 2 * sizeof(int), &numPathGroup, sizeof(int));
  memcpy(msg->data + 3 * sizeof(double) + 3 * sizeof(int), &T, sizeof(double));
  return msg;
}
