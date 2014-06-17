/** End to end test for option pricing */
#include "TestLib.hpp"

#include <client_api.hpp>
#include <iostream>
#include <Logging.hpp>

int main(int argc, char* argv[])
{
  initLogging("OptionPricingTest.log");

  /** Ask for a simple option pricings a few times */
  int t = 1;

  double strike = 0.01;
  double sigma = 0.02;
  double timestep = 0.05;
  int numTimeStep = (int)(10/0.05);
  int numMaturity = 2000000;
  int paraNode = 50;
  int numPathGroup = 20;
  double T = 10;

  bool status = true;
  for (int k = 0; k < t; k++) {
    double res;
    optionPricing(strike,
  		  sigma,
  		  timestep,
  		  numMaturity,
  		  paraNode,
  		  numPathGroup,
  		  T,
  		  &res);
    printf("res = %d\n", res);
  }

  return status ? 0 : 1;
}
