#include <Scheduler.hpp>

#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <Logging.hpp>

using namespace std;

int main(int argc, char** argv) {

  initLogging("scheduler.log");

  if (argc < 3) {
    cout << "Usage ./scheduler <portNumber> <dispatcherPortNumber>" << endl;
    return 1;
  }

  int dispatcherPortNumber = atoi(argv[2]);

  auto localhost = string("localhost");

  auto elasticityManager = ElasticityManager();

  Scheduler s(elasticityManager,
	      argv[1],
	      localhost,
	      dispatcherPortNumber,
	      localhost);

  /* Add some scheduling algorithms */
  /* 0 */
  s.addSchedAlg(FCFSMax);
  /* 1 */
  s.addSchedAlg(FCFSMin);
  /* 2 */
  s.addSchedAlg(FCFSAsManyAsPos);
  /* 3 */
  s.addSchedAlg(SJTF);
  /* 4 */
  s.addSchedAlg(Priority);
  /* 5 */
  s.addSchedAlg(ManagedMode);

  s.start();

  return 0;
}
