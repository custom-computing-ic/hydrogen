#include <Scheduler.hpp>

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {

  if (argc < 3) {
    cout << "Usage ./scheduler <portNumber> <dispatcherPortNumber>" << endl;
    return 1;
  }

  int portnumber = atoi(argv[1]);

  const string& name = string("Scheduler");
  Scheduler s(portnumber, name);

  s.start();
}
