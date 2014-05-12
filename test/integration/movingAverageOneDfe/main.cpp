/** End to end test for moving average */
#include <client.hpp>

using namespace std;

int main() {

  int data[] = {1, 2, 3, 4};

  /** Ask for a simple moving average a few times */
  int t = 10;

  for (int i = 0; i < t; i++) {
    movingAverage(4, data);
  }

  return 0;

}
