/** Stub for firing off an add Resource request */
#include "TestLib.hpp"
#include <client_api.hpp>
#include <iostream>
#include <cstring>

using namespace std;

int main() {
  bool status = true;
  addResource("CPU","localhost","8118",4);
  sleep(10);
  return status ? 0 : 1;
}
