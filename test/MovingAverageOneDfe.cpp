/** End to end test for moving average */
#include <client_api.hpp>

#include <iostream>
#include <cstring>

using namespace std;

int main() {

  int n = 16;

  int *data = (int *)calloc(n, sizeof(int));
  for (int i = 0; i < n; i ++)
    data[i] = i;

  int *exp = (int *)calloc(n, sizeof(int));
  for (int i = 1; i < n - 1; i ++)
    exp[i] = (data[i - 1] + data[i] + data[i + 1]) / 3;


  /** Ask for a simple moving average a few times */
  int t = 1;
  int out[n];
  memset(out, 0, sizeof(int) * n);

  int status = 0;
  for (int k = 0; k < t; k++) {
    movingAverage(n, 3, data, out);
    for (int i = 1; i < n - 1; i++) {
      cout << "Checking data " << endl;
      if (out[i] != exp[i]) {
        cout << "Error " << i << " expected: " << exp[i] << " got: " << out[i];
        status = -1;
      }
    }
  }

  return status;
}
