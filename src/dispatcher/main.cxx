#include <Dispatcher.hpp>

#include <string>

using namespace std;

int main() {

  int portnumber = 8111;

  const string& name = string("Dispatcher - maxnode1");
  Dispatcher d(portnumber, name);

  d.start();

  return 0;
}
