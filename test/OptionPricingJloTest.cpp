/** End to end test for option pricing with elasticity.  */
#include "TestLib.hpp"

#include <client_api.hpp>
#include <iostream>
#include <Logging.hpp>

int main()
{
  initLogging("OptionPricingTest.log");

  double strike = 0.01;
  double sigma = 0.02;
  double timestep = 0.05;
  int numTimeStep = (int)(10/0.05);
  int numMaturity = 12;
  int paraNode = 50;
  int numPathGroup = 100000;
  double T = 10;

  // predicted time for single DFE execution
  // TODO this should normally come from the executor
  int perfModel[] = {
    6000, // numPathGroup  = 1000000
    12000, // 3000000
    18000,
  };

  int repetitions = 2;
  bool status = true;

  LOG(debug) << "Starting scale test";
  // test resource allocation scales up
  int factor = 10;
  for (; factor < 30; factor+=10) {
    for (int k = 0; k < repetitions; k++) {
      double res;
      int newNumPathGroup = numPathGroup * factor;
      LOGF(debug, "numPathGroup = %1% k = %2%") % newNumPathGroup % k;
      optionPricingJlo(strike,
		       sigma,
		       timestep,
		       numMaturity,
		       paraNode,
		       newNumPathGroup,
		       T,
		       &res,
		       perfModel[factor / 10],
		       4000
		       );
      printf("res = %d\n", res);
   }
  }

  LOG(debug) << "Starting scale down test";
  // test resource allocation scales down
  for (; factor > 10; factor -= 10) {
    for (int k = 0; k < repetitions; k++) {
      double res;
      int newNumPathGroup = numPathGroup * factor;
      LOGF(debug, "numPathGroup = %1% k = %2%") % newNumPathGroup % k;
      optionPricingJlo(strike,
		       sigma,
		       timestep,
		       numMaturity,
		       paraNode,
		       newNumPathGroup,
		       T,
		       &res,
		       perfModel[factor / 10],
		       4000
		       );
      printf("res = %d\n", res);
    }
  }

  return status ? 0 : 1;
}
