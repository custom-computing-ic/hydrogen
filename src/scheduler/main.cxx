#include <Scheduler.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>

using namespace std;

Scheduler *s;

void kill_handler(int sig) {
  cout << "Caught kill signal. Cleaning up. Please wait..." << endl;
  if (s)
    s->stop();
}

int main(int argc, char** argv) {

  if (argc < 3) {
    cout << "Usage ./scheduler <portNumber> <dispatcherPortNumber>" << endl;
    return 1;
  }

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = kill_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  int portnumber = atoi(argv[1]);
  int dispatcherPortNumber = atoi(argv[2]);

  const string& name = "Scheduler";
  const string& dispatcherHostname = "localhost";

  s = new Scheduler(portnumber, name, dispatcherPortNumber, dispatcherHostname);
  s->start();

  return 0;
}
