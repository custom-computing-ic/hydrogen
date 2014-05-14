#include <client_api.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {

  int n = 10;
  for (int i = 0; i < n; i++) {
    int pid = fork();
    if (pid == 0) {
      int data[] = {1, 2, 3, 4, 5, 6};
      int exp[]  = {2, 3, 4, 5, 0, 0};

      int out[6];
      memset(out, 0, sizeof(int) * 6);

      int status = 0;
      movingAverage(6, 3, data, out);
      for (int i = 0; i < 6; i++) {
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
  for (int i = 0; i < n; i++ ) {
    wait(NULL);
  }
}
