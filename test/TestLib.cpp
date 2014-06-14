#include "TestLib.hpp"

#include <cstdlib>
#include <iostream>

using namespace std;

int* mavg_data(int n) {
  int *a = (int *)calloc(n, sizeof(int));
  for (int i = 0; i < n; i++)
    a[i] = i + 1;
  return a;
}

int* mavg_threept_exp(int n, int* a) {
  int* exp = (int *)calloc(n, sizeof(int));
  for (int i = 1; i < n - 1; i++)
    exp[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;

  exp[0] = (a[0] + a[1]) / 3;
  exp[n - 1] = (a[n - 1] + a[n - 2]) / 3;
  return exp;
}

bool mavg_check(int n, int* out, int* exp) {
  int correct = true;
  for (int i = 1; i < n - 1; i++) {
    if (out[i] != exp[i]) {
      cout << "(ERROR): " << i << " expected: " << exp[i] << " got: " << out[i];
      cout << endl;
      correct = false;
    }
  }
  if (!correct) {
    cout << "(ERROR): Moving Average Returned incorrect Data";
    cout << endl;
  }
  return correct;
}
