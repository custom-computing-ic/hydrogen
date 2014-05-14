/** End to end test for moving average */
#include <client_api.hpp>

#include <iostream>
#include <cstring>

using namespace std;

int main() {

  int data[] = {1, 2, 3, 4, 5, 6};
  int exp[]  = {2, 3, 4, 5, 0, 0};

  /** Ask for a simple moving average a few times */
  int t = 10;
  int out[6];
  memset(out, 0, sizeof(int) * 6);

  int status = 0;
  for (int k = 0; k < t; k++) {
    movingAverage(6, 3, data, out);
    for (int i = 0; i < 6; i++) {
      cout << "Checking data " << endl;
      if (out[i] != exp[i]) {
        cout << "Error " << i << " expected: " << exp[i] << " got: " << out[i];
        status = -1;
      }
    }
  }

  return status;
}
