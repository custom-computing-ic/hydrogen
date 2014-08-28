#include "TestLib.hpp"
#include "concurrent_deque.hpp"

int main() {

  concurrent_deque<int> cd;

  cd.push_back(1);
  cd.push_back(2);
  cd.push_front(3);
  cd.push_front(4);

  bool status = true;

  int last = cd.wait_pop_back();
  ASSERT(last == 2);
  ASSERT(cd.size() == 3);

  return status ? 0 : 1;
}
