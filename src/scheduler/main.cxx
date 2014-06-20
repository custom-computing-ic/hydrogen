#include <Scheduler.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <cstdlib>
#include <csignal>

using namespace std;
namespace po = boost::program_options;


int main(int argc, char** argv) {
  po::options_description desc("Program options");
  desc.add_options()
    ("help,h", "Produce this help message")
    ("scheduler_port,p", po::value<int>(), "Scheduler Port - Listen for incomming connections on this port (default 8111)")
    ("scheduler_host,n", po::value<std::string>(), "Hostname - Host address to bind to  (default localhost)")
    ("dispatcher_port,d", po::value<int>(), "Dispatcher Port - (defualt 8112)")
    ("dispatcher_host,w", po::value<std::string>(), "Dispatcher Hostname - (default localhost)")
    ("use_gui,g", po::bool_switch()->default_value(false),"Use graphical interface")
    ;

   po::variables_map vm;
   po::store(po::parse_command_line(argc,argv,desc),vm);
   po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  bool useGUI = false;
  string schedulerHost = string("localhost");
  string schedulerPortNumber = string("8111");
  string dispatcherHost = string("localhost");
  int dispatcherPortNumber = 8112;
  if (vm.count("scheduler_host")) {
    schedulerHost = vm["scheduler_host"].as<string>();
  }
  if (vm.count("scheduler_port")) {
    schedulerPortNumber = vm["scheduler_port"].as<string>();
  }
  if (vm.count("dispatcher_host")) {
    dispatcherHost = vm["dispatcher_host"].as<string>();
  }

  if (vm.count("dispatcher_port")) {
    dispatcherPortNumber = vm["dispatcher_port"].as<int>();
  }
  if (vm.count("use_gui")){
    useGUI = vm["use_gui"].as<bool>();
  }

  cout << "\n(INFO): Binding to: " << schedulerHost << ":";
  cout << schedulerPortNumber << "\n";
  cout << "\n(INFO): Dispatcher: " << dispatcherHost << ":";
  cout << dispatcherPortNumber << "\n";
  Scheduler s(schedulerPortNumber,
              schedulerHost,
              dispatcherPortNumber,
              dispatcherHost,
              useGUI);

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
  try {
    s.start();
  } catch (std::exception& e) {
    cout << "\nError: " << e.what() << endl;
  }

  return 0;
}
