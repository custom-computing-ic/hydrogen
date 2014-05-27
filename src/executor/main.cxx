#include <iostream>
#include <functional>

#include <utils.hpp>
//#include "timer/timer.h"
//#include "timer/simpleTimer.h"
#include <SimpleTimer.hpp>
#include <Executor.hpp>
#include <Implementation.hpp>
#include <boost/timer/timer.hpp>
#include <random>
#include <sstream>
#include <boost/program_options.hpp>
//#include "build/gen/control.pb.h"
#define MSIZE 2

namespace po = boost::program_options;





int main(int argc, char** argv) {
  po::options_description desc("Program options");
  desc.add_options()
     ("help,h", "produce help message")
     ("client_id,c", po::value<std::string>(), "set client id")
     ("scheduler_port,p", po::value<int>(), "scheduler port")
     ("hostname,n", po::value<std::string>(), "scheduler hostname")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc), vm);
  po::notify(vm);
  if (vm.count("help") || vm.size() < 3) {
    std::cout << desc << "\n";
    return 1;
  }


  Executor e("localhost", vm["client_id"].as<std::string>());
  /* Add a local CPU and the shared DFE's from the cmd line */
  e.AddResource(new Resource(1, vm["scheduler_port"].as<int>(),
                             vm["hostname"].as<std::string>(), 
                             "SHARED_DFE"));
  e.AddResource(new Resource(2, 8114, "localhost", "CPU"));
  /* Adding some tasks... */
  e.AddTask(new Task("MOVING_AVERAGE"));
  e.AddImp(e.FindTask("MOVING_AVERAGE"), 
           new Implementation("","","MOVING_AVERAGE","","","DFE"));
  e.AddImp(e.FindTask("MOVING_AVERAGE"),
           new Implementation( "","","MOVING_AVERAGE","", "","CPU"));
 



  e.start();
  return 0;
}


/* Adding tasks */
/*  e.AddTask(new Task("MatrixMultiply"));
  Task *t = e.FindTask("MatrixMultiply");	

  if (t) {
    std::cout << *t << std::endl;
  } else {
    std::cout << "Couldn't find task\n";
  }
*/



/*  perf.CreateModel(500,[&](const int size) -> double {
                    if (size == 0)
                      return 0.0;

                    std::default_random_engine gen;
                    std::uniform_real_distribution<double> dist(0.0,10000.0);
                    auto a1 = (double*) malloc(size*size*sizeof(double));
                    auto b1 = (double*) malloc(size*size*sizeof(double));
                    auto c1 = (double*) malloc(size*size*sizeof(double));
                    for (int i = 0; i < size; i++) {
                      for (int j = 0; j < size; j++) {
                        a1[i*size + j] = dist(gen);
                        b1[i*size + j] = dist(gen);
                      }
                    }              
                    SimpleTimer test_t;
                    test_t.start();
                    perf.imp->run<double>(size,a1,b1,c1);
                    uint64_t ms = test_t.end();
                    std::stringstream ss;
                    ss << ms;
                    delete a1;
                    delete b1;
                    delete c1;
                    double msd;
                    ss >> msd;
                    return msd;
                  });
*/

