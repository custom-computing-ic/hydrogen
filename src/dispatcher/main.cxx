#include <Dispatcher.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>

using namespace std;

int main(int argc, char** argv) {

  if (argc < 4) {
    cout << "Usage ./dispatcher <portNumber> <useGroups (1/0)> <Default #DFEs[1..4]>" << endl;
    return 1;
  }

  int portnumber = atoi(argv[1]);
  int groups = atoi(argv[2]);
  int nDfes = atoi(argv[3]);
  if (nDfes > 4 || nDfes < 1)
     nDfes = 1;

  const string& localhost = string("localhost");
  Dispatcher* d = nullptr;
  try {
    d = new Dispatcher(portnumber, localhost, groups == 1, nDfes);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    if (d != nullptr)
      delete d;
    return 1;
  }

  //  d->start();
  return 0;
}
