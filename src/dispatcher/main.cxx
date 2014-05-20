#include <Dispatcher.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>

using namespace std;

int main(int argc, char** argv) {

  if (argc < 2) {
    cout << "Usage ./scheduler <portNumber>" << endl;
    return 1;
  }

  int portnumber = atoi(argv[1]);
  Dispatcher d(portnumber, "localhost");
  //  d->start();
  return 0;
}
