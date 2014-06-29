/** End to end test for option pricing with elasticity.  */
#include "TestLib.hpp"

#include <client_api.hpp>
#include <iostream>
#include <Logging.hpp>

#include <boost/chrono.hpp>

namespace bc = boost::chrono;

int main()
{
  initLogging("OptionPricingTest.log");

  double strike = 0.01;
  double sigma = 0.02;
  double timestep = 0.05;
  int numTimeStep = (int)(10/0.05);
  int numMaturity = 12;
  int paraNode = 50;
  double T = 10;

  // how many requests to send
  int requestCount = 2;

  // measured execution times
  int perfModel[] = {
    5910,
    5910,
    11781,
    11781,
    17712,
    17712,
    11781,
    11781,
    5910,
    5910
      };

  // million paths
  int request[] = {
    1,
    1,
    2,
    2,
    3,
    3,
      2,
      2,
      1,
      1
      };

  int target[] = {
    5000,
    5000,
    5000,
    5000,
    16000,
    16000,
    11000,
    11000,
    5000,
    5000
   };

  bool status = true;

  for (int i = 0; i < requestCount; i++) {
      int numPathGroup = 1E6 * request[i];
      LOGF(debug, "numPathGroup = %1%") % numPathGroup;
      auto start = bc::system_clock::now();
double res;
      optionPricingJlo(strike,
		       sigma,
		       timestep,
		       numMaturity,
		       paraNode,
            	       numPathGroup,
		       T,
		       &res,
		       perfModel[i],
		       target[i]
		       );
      auto end = bc::system_clock::now();
      LOG(debug) << "  Took: " << bc::duration_cast<bc::milliseconds>(end - start).count();
     LOG(debug) << "  res = " << res;
  }

  return status ? 0 : 1;
}
