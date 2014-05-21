#include "TestLib.hpp"
#include <client_api.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {

  int n = 16;
  int *data = mavg_data(n);
  int *exp = mavg_threept_exp(n, data);

  int p = 10;
  for (int i = 0; i < p; i++) {
    int pid = fork();

    if (pid == 0) {
      // child process
      int out[n];
      memset(out, 0, sizeof(int) * n);
      movingAverage(n, 3, data, out);

      exit(mavg_check(n, out, exp)? 0 : 1);
    } else if (pid < 0) {
      // parent process
      cout << "Failed to fork" << endl;
    }
  }

  // wait for all children to finish and check status
  int status = 0;
  for (int i = 0; i < p; i++ ) {
    int st = 0;
    wait(&st);
    std::cout << "Process status:" << std::endl;
    cout << st << endl;
    status = status || st;
  }

  return status;
}
