#include <iostream>
#include <stdlib.h>
#include <functional>

#include <utils.hpp>
//#include "timer/timer.h"
//#include "timer/simpleTimer.h"
#include <SimpleTimer.hpp>
#include <Executor.hpp>
#include <Implementation.hpp>
#include <client_api.hpp>
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
  char* clientIdCh;
  int client_id = -1;
  clientIdCh = getenv("CLIENT_ID");
  if (clientIdCh == NULL) {
    std::cout << "(ERROR): Environmental Variable CLIENT_ID must be set before"
              << " running the Executor service.\n"
              << "(ERROR): Using default from the command line: ";
    clientIdCh = strdup(vm["client_id"].as<std::string>().c_str());
    std::cout << clientIdCh << "\n";
  } else {
    client_id = atoi(clientIdCh);
    std::cout << "(DEBUG): Environmental Variable CLIENT_ID = " << client_id << "\n";
  }
  
  Executor e("localhost",clientIdCh);  
  /* Add a local CPU and the shared DFE's from the cmd line */
  e.AddResource(new Resource(1, vm["scheduler_port"].as<int>(),
                             vm["hostname"].as<std::string>(), 
                             "SHARED_DFE"));
  e.AddResource(new Resource(2, 8114, "localhost", "CPU"));
  /* Adding some tasks... */
  e.AddTask(new Task("MOVING_AVERAGE"));
  Implementation *mav_DFE = new Implementation("MAV","mavDFE","MOVING_AVERAGE","","","SHARED_DFE");
  PerfModel perf(mav_DFE,4e-18,1e2,2,1);
  /*
  perf.CreateModel(38400000, [&](const int size) -> double {
                  if (size == 0) 
                    return 0.0;
                  std::default_random_engine gen;
                  std::uniform_int_distribution<int> dist(0,10000);
                  auto a1 = (int*) malloc(size*sizeof(int));
                  auto out =(int*) malloc(size*sizeof(int));
                  for (int j = 0; j < size; j++) {
                    a1[j] = dist(gen);
                  }
                  SimpleTimer test_t;
                  test_t.start();
                  movingAverage(size,3,a1,out,1);
                  uint64_t ms = test_t.end();
                  std::stringstream ss;
                  std::cout << "(INFO): Size: " << size << "\t ms: " << ms << "\n";
                  ss << ms;
                  free(a1);
                  free(out);
                  double msd;
                  ss >> msd;
                  return msd;
                });*/
 // std::cout << "(INFO)" << perf << "\n";
  perf.LoadFromDisk("MAV_DFE");
  std::cout << "(INFO)" << perf << "\n";

  for (int i = 0; i < 38400000; i+= 384000) {
    std::cout << "(DEBUG): perf.QueryModel("<< i 
              << ") = " << perf.QueryModel(i) << "ms\n";
  }
  perf.SaveToDisk("MAV_DFE");
  e.AddImp(e.FindTask("MOVING_AVERAGE"), mav_DFE);
//  e.AddImp(e.FindTask("MOVING_AVERAGE"),
//           new Implementation( "MAV","mavCPU","MOVING_AVERAGE","", "","CPU"));
 


//  e.start();
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

