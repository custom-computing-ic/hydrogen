#include <Dispatcher.hpp>

using namespace std;

int main() {

  Dispatcher d;

  int portnumber = 8111;
  d.start(portnumber);

  return 0;
}
