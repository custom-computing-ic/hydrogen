#include <message.hpp>
#include <cstring>

void msg::print() {
  using namespace std;
  cout << "  Id: " << msgId;
  cout << "  Data size: " << dataSize << endl;
  cout << "  Params size: " << paramsSize << endl;
  cout << "  Exps data size bytes: " << expDataSizeBytes << endl;
  cout << "  Data: ";
  cout << "  Total bytes: " << totalBytes << endl;
  cout << "  Data Bytes: " << dataBytes << endl;
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


int msg::firstParam() {
  return *(data + dataSize * sizeof(int));
}


msg_t* msg_empty() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  msg->totalBytes = sizeof(msg_t);
  msg->dataBytes = 0;
  return msg;
}

msg_t* msg_ack() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  msg->msgId = MSG_ACK;
  msg->totalBytes = sizeof(msg_t);
  msg->dataBytes = 0;
  return msg;
}
msg_t* msg_done() {
  msg_t *msg = (msg_t *)calloc(sizeof(msg_t), 1);
  msg->msgId = MSG_DONE;
  msg->totalBytes = sizeof(msg_t);
  msg->dataBytes = 0;
  return msg;
}

msg_t* msg_moving_avg(int n, size_t width, int* dataIn) {
  int dataBytes = n * sizeof(int) + sizeof(size_t);
  int sizeBytes = sizeof(msg_t) + dataBytes;
  msg_t* msg = (msg_t *)calloc(sizeBytes, 1);
  msg->msgId = MSG_MOVING_AVG;
  msg->expDataSizeBytes = n * sizeof(int);
  msg->dataSize = n;
  msg->paramsSize = 1;
  msg->totalBytes = sizeBytes;
  msg->dataBytes = dataBytes;
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
  int dataBytes = 4 * sizeof(double) + 3 * sizeof(int);
  int sizeBytes = sizeof(msg_t) + dataBytes;
  msg_t* msg = (msg_t *)calloc(sizeBytes, 1);
  msg->msgId = MSG_OPTION_PRICE;
  msg->dataSize = 0;
  msg->expDataSizeBytes = sizeof(double);
  msg->paramsSize = 7;
  msg->dataBytes = dataBytes;
  msg->totalBytes = sizeBytes;

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
