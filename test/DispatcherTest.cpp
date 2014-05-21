#include <Dispatcher.hpp>
#include <Client.hpp>
#include <message.hpp>

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <cstring>

using boost::asio::ip::tcp;
using namespace std;

int main() {
  msg_t* msg = NULL;
  int data_in[] = {1, 2, 3, 4};
  int exp[] = {2, 3, 0, 0};
  int *out = (int *)malloc(4 * sizeof(int));

  try {
    Client c(8112, "localhost");
    c.start();
    char buf[1024];
    boost::system::error_code error;

    int sizeBytes = sizeof(msg_t) + 5 * sizeof(int);
    msg = (msg_t *)calloc(sizeBytes, 1);
    msg->msgId = MSG_MOVING_AVG;
    msg->dataSize = 4;
    msg->paramsSize = 1;
    memcpy(msg->data, data_in, sizeof(int) * msg->dataSize);
    *(msg->data + sizeof(int) * msg->dataSize) = 3;
    memcpy(buf, msg, sizeBytes);

    c.send(msg);
    c.getResult(out);
    c.stop();
    msg->print();

  } catch (const exception& e) {
    cerr << "Caught exception" << endl;
    cerr << e.what() << endl;
    return 1;
  }

 
  return msg && (memcmp(exp, out, 4 * sizeof(int)) == 0) ? 0 : 1;
}
