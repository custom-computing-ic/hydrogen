#include "TestLib.hpp"

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
  int n = mavg_data_size;
  int *a = mavg_data(n);
  int *out = (int *)calloc(n, sizeof(int));
  int *exp = mavg_threept_exp(n, a);

  try {
    Client c(8112, "localhost");
    c.start();

    msg_t* msg = msg_moving_avg(n, 3, a);
    char buf[msg->sizeBytes()];
    memcpy(buf, msg, msg->sizeBytes());

    c.send(msg);
    c.getResult(out);
    c.stop();

  } catch (const exception& e) {
    cerr << "Caught exception" << endl;
    cerr << e.what() << endl;
    return 1;
  }

  return mavg_check(n, out, exp)? 0 : 1;
}
