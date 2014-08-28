#include <Dispatcher.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <Logging.hpp>

using namespace std;

int main(int argc, char** argv) {

  if (argc < 2) {
    cout << "Usage ./dispatcher <portNumber>" << endl;
    return 1;
  }

  initLogging("dispatcher.log");

  int portnumber = atoi(argv[1]);
  const string& localhost = string("localhost");
  Dispatcher d(portnumber, localhost);
  //  d->start();
  return 0;
}
