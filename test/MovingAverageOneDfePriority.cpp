/** End to end test for moving average */
#include "TestLib.hpp"

#include <client_api.hpp>

#include <iostream>
#include <cstring>
#include <boost/chrono.hpp>
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

  auto estart = boost::chrono::system_clock::now();
  int *exp = mavg_threept_exp(n, data);
  auto eend = boost::chrono::system_clock::now();
  boost::chrono::duration<double> edur = eend - estart;
  cout << "CPU time: " << edur << "\n";

  /** Ask for a simple moving average a few times */
  int t = 10;
  int *out = (int*)malloc(n * sizeof(int));
  bool status = true;
  for (int k = 0; k < t; k++) {
    memset(out, 0, sizeof(int) * n);
    auto start = boost::chrono::system_clock::now();
    movingAverage(n, 3, data, out,cid);
    auto end = boost::chrono::system_clock::now();
    boost::chrono::duration<double> dur = end - start;
    cout << "DFE time: " << dur << "\n";
    status &= mavg_check(n, out, exp);
  }

  return status ? 0 : 1;
}
