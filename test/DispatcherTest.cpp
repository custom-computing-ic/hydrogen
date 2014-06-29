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
    const string& host = "localhost";
    Client c(8112, host);
    c.start();

    msg_t* msg = msg_moving_avg(n, 3, a);
    char* buf = (char* )calloc(msg->totalBytes, 1);
    memcpy(buf, msg, msg->totalBytes);
    c.send(msg);
    c.getResult(out, n * sizeof(int));
    c.stop();
    free(buf);
  } catch (const exception& e) {
    cerr << "Caught exception" << endl;
    cerr << e.what() << endl;
    return 1;
  }

  return mavg_check(n, out, exp)? 0 : 1;
}
