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
  int n = 16;

  int *a = (int *)calloc(n, sizeof(int));
  int* exp = (int *)calloc(n, sizeof(int));
  for (int i = 0; i < n; i++)
    a[i] = i + 1;
  for (int i = 1; i < n - 1; i++)
    exp[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;

  exp[0] = (a[0] + a[1]) / 3;
  exp[n - 1] = (a[n - 1] + a[n - 2]) / 3;

  int *out = (int *)malloc(n * sizeof(int));

  try {
    Client c(8112, "localhost");
    c.start();

    boost::system::error_code error;

    msg_t* msg = msg_moving_avg(n, 3, a);
    char buf[msg->sizeBytes()];
    memcpy(buf, msg, msg->sizeBytes());

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
