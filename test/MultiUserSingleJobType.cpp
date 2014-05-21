#include <client_api.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {

  int p = 10;
  for (int i = 0; i < p; i++) {
    int pid = fork();
    if (pid == 0) {

      int n = 16;

      int *data = (int *)calloc(n, sizeof(int));
      for (int i = 0; i < n; i ++)
	data[i] = i;

      int *exp = (int *)calloc(n, sizeof(int));
      for (int i = 1; i < n - 1; i ++)
	exp[i] = (data[i - 1] + data[i] + data[i + 1]) / 3;

      int out[n];
      memset(out, 0, sizeof(int) * n);

      int status = 0;
      movingAverage(n, 3, data, out);
      for (int i = 1; i < n - 1; i++) {
        if (out[i] != exp[i]) {
          cout << "Error " << i << " expected: " << exp[i] << " got: " << out[i];
          status = -1;
        }
      }
      cout << "Checked result" << endl;
      exit(0);
    } else if (pid < 0) {
      cout << "Failed to fork" << endl;
    }
  }

  // wait for all children to finish
  for (int i = 0; i < p; i++ ) {
    wait(NULL);
  }
}
