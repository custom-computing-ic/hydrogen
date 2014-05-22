/** End to end test for moving average */
#include "TestLib.hpp"

#include <client_api.hpp>

#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {

  if (argc != 2) {
    std::cout << "Error: incorrect client id.\n"
              << "\tUsage ./MovingAverageOneDfePriority <clientId>\n";
    return 1;
  }
  int cid = atoi(argv[1]);
  int n = mavg_data_size;

  int *data = mavg_data(n);
  int *exp = mavg_threept_exp(n, data);

  /** Ask for a simple moving average a few times */
  int t = 10;

  bool status = true;
  for (int k = 0; k < t; k++) {
    int out[n];
    memset(out, 0, sizeof(int) * n);
    movingAverage(n, 3, data, out,cid);
    status &= mavg_check(n, out, exp);
  }

  return status ? 0 : 1;
}
