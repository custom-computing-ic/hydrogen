/** End to end test for moving average */
#include "TestLib.hpp"

#include <client_api.hpp>

#include <iostream>
#include <cstring>

using namespace std;

int main() {

  int n = 16;

  int *data = mavg_data(n);
  int *exp = mavg_threept_exp(n, data);

  /** Ask for a simple moving average a few times */
  int t = 10;

  bool status = true;
  for (int k = 0; k < t; k++) {
    int out[n];
    memset(out, 0, sizeof(int) * n);
    movingAverage(n, 3, data, out);
    status &= mavg_check(n, out, exp);
  }

  return status ? 0 : 1;
}
