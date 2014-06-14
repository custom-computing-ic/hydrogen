#include "TestLib.hpp"
#include <client_api.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {

  int n = mavg_data_size;
  int *data = mavg_data(n);
  int *exp = mavg_threept_exp(n, data);

  int p = 2;
  int numJobs = 100;
  for (int i = 0; i < p; i++) {
    int pid = fork();

    if (pid == 0) {
      // child process
         bool correct = true;
      for (int j = 0; j < numJobs; j++) {
        int* out = (int*)calloc(n, sizeof(int));
        movingAverage(n, 3, data, out);
        correct &= mavg_check(n, out, exp);
        free(out);
      }
      exit(correct ? EXIT_SUCCESS : EXIT_FAILURE);
    } else if (pid < 0) {
      // parent process
      cout << "Failed to fork" << endl;
    }
  }

  // wait for all children to finish and check status
  bool ok = true;
  for (int i = 0; i < p; i++ ) {
    int st = 0;
    wait(&st);
    ok = ok && (st == EXIT_SUCCESS);
  }

  return ok ? 0 : 1;
}
