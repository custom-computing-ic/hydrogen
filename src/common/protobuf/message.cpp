#include <message.hpp>
#include <cstring>

void msg::print_control() {
   using namespace std;
   cout << "ControlMSG\t";
   cout << "  Id: " << msgId;

   for (size_t i = 0; i < dataSize*sizeof(int); i++) {
//    char* c_data = reinterpret_cast<char *>(data);
    cout << data[i];
   }
   cout << endl;

}

void msg::print() {
  using namespace std;
  if (msgId == MSG_CONTROL) {
    print_control();
    return;
  }
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

msg_t* msg_control_add_res(std::string type,
                           std::string host,
                           std::string port) {
//  GOOGLE_PROTOBUF_VERIFY_VERSION;
  ControlMSG c;
  c.set_command("ADD_RESOURCE");
  c.set_arg1(host);
  c.set_arg2(port);
  c.set_arg3(type);
  int sizeBytes = c.ByteSize();      
  msg_t* msg = (msg_t *)calloc(sizeBytes + sizeof(msg_t),0);
  c.SerializeToArray(msg->data,sizeBytes);

  msg->msgId = MSG_CONTROL;
  msg->dataSize = sizeBytes / sizeof(int); //MASSIVE HACK
  msg->paramsSize = 0;
//  google::protobuf::ShutdownProtobufLibrary();

  return msg;
}

msg_t* msg_control_rm_res() {
  return NULL;
}

msg_t* msg_control_add_imp() {
  return NULL;
}

msg_t* msg_control_rm_imp() {
  return NULL;
}


