/** Stub for firing off an add Resource request */
#include "TestLib.hpp"
#include <client_api.hpp>
#include <iostream>
#include <cstring>
#include <packet.pb.h>
using namespace std;

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  bool status = true;
  try {
    const std::string& type = string("CPU");
    const std::string& host = string("localhost");
    const std::string& port = string("8118");
    int cid = 4;
    addResource(type,host,port,cid);
  } catch (std::exception& e) {
    cout << e.what() << endl;
  }
  google::protobuf::ShutdownProtobufLibrary();
  return status ? 0 : 1;
}
