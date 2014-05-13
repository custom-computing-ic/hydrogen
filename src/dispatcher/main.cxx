#include <Dispatcher.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>

using namespace std;

Dispatcher* d;

void kill_handler(int s) {
  cout << "Caught kill signal. Cleaning up. Please wait..." << endl;
  if (d)
    d->stop();
}

int main(int argc, char** argv) {

  if (argc < 2) {
    cout << "Usage ./scheduler <portNumber>" << endl;
    return 1;
  }

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = kill_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  int portnumber = atoi(argv[1]);

  const string& name = string("Dispatcher");
  d = new Dispatcher(portnumber, name);
  d->start();
  return 0;
}
