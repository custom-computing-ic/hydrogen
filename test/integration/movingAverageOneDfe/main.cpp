/** End to end test for moving average */
#include <client_api.hpp>

using namespace std;

int main() {

  int data[] = {1, 2, 3, 4, 5, 6};

  /** Ask for a simple moving average a few times */
  int t = 2;

  for (int i = 0; i < t; i++) {
    movingAverage(6, data);
  }

  return 0;

}
