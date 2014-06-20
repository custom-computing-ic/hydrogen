#include <iostream>
#include <stdlib.h>
#include <functional>
#include <boost/iostreams/device/mapped_file.hpp>
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
const long int TSIZE = 3840000;

namespace po = boost::program_options;
namespace bio = boost::iostreams;




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

  Executor* e = nullptr;
  try {
    e = new Executor("localhost",clientIdCh);
  } catch (std::exception& err) {
    std::cerr << "\nError: " << err.what() << std::endl;
    if (e != nullptr)
      delete e;
    return 1;
  }

  /* Add a local CPU and the shared DFE's from the cmd line */
  e->AddResource(new Resource(1, vm["scheduler_port"].as<int>(),
                             vm["hostname"].as<std::string>(),
                             "SHARED_DFE"));
  e->AddResource(new Resource(2, 8112, "localhost", "CPU"));
  /* Adding some tasks... */
  e->AddTask(new Task("MOVING_AVERAGE"));
  Implementation *mav_DFE = new Implementation("MAV","mavDFE","MOVING_AVERAGE","","","SHARED_DFE");
  Implementation *mav_CPU = new Implementation("MAV","mavCPU","MOVING_AVERAGE","","","CPU");

  PerfModel* perf_CPU = new PerfModel(mav_CPU,4e-18,1e4,2,1);
  PerfModel* perf_DFE = new PerfModel(mav_DFE,1e-19,1e6,2,1);

  std::default_random_engine gen;
/*  perf_DFE->CreateModel(384000000, [&](const long size) -> double {
                  if (size == 0)
                    return 0.0;
                  auto param = bio::mapped_file_params("/mnt/data/cccad3/mt3410/mav.dat");
                  auto a1 = bio::mapped_file_source(param);
                  if (!a1.is_open())
                    a1.open(param);
                 if (a1.size() < (size*sizeof(int))) {
                    std::cerr << "(ERROR): Not enough data for this run"
                              << "producing -1 as runtime.";
                    return -1.0;
                  }
                  auto out =(int*) malloc(size*sizeof(int));
                  std::uniform_int_distribution<int> dist(0,a1.size()-TSIZE*sizeof(int));
                  std::uniform_int_distribution<int> dist(0,a1.size()-size*sizeof(int));

                  auto offset = dist(gen);
                  auto input = a1.data() + offset;
                  std::cout << "(INFO): size: " << a1.size() << "\toffset:"
                            << offset << "\n";
                  SimpleTimer test_t;
                  test_t.start();
                  movingAverage(size,3,(int*)input,out,2);
                  uint64_t ms = test_t.end();
                  std::stringstream ss;
                  auto mb = (((size*sizeof(int))/1024)/1024);
                  std::cout << "(INFO): Size: " << size << " (" << mb << "MB)"
                            << "\t ms: " << ms << "\n";
                  ss << ms;
                  free(out);
                  double msd;
                  ss >> msd;
                  return msd;
                });*/
//  std::cout << "(INFO): " << *perf_DFE << "\n";
//  perf_DFE->SaveToDisk("MAV_DFE_4");
  perf_CPU->LoadFromDisk("MAV_CPU");
  perf_DFE->LoadFromDisk("MAV_DFE");
  std::cout << "(INFO): " << *perf_CPU << "\n";
  std::cout << "(INFO): " << *perf_DFE << "\n";

  perf_DFE->setAlpha(1e-19);
  perf_DFE->setIter(1e6);
  perf_DFE->LinearRegression();
  std::cout << "\n(INFO)" << *perf_DFE << "\n";
  //perf_DFE->SaveToDisk("MAV_DFE");
  e->AddImp(e->FindTask("MOVING_AVERAGE"), perf_DFE);
  e->AddImp(e->FindTask("MOVING_AVERAGE"), perf_CPU);
  e->start();
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

