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
//#include "build/gen/control.pb.h"
#define MSIZE 2


int main(int argc, char** argv) {
  cmd_args_t args;
  parse_cmd(argc,argv,&args); 

  Executor e(args.execPort->getValue(),
             "localhost",
             args.schedPort->getValue(),
             args.hostName->getValue(),
             args.clientId->getValue());
  /*loading implementations from libraries */
//  Implementation *imp = new Implementation("./libMM.so","mmult");
  /* Checking for error loading the library */
//  if (imp->load() == NULL) {
//    std::cout << "Exiting" << std::endl;
//    return 1;
//  }
//  Implementation *print_matrix = new Implementation("./libMM.so","print_matrix");
//  if (print_matrix->load() == NULL) {
//    std::cout << "Exiting" << std::endl;
//    return 1;
//  }


/*
  auto a =  new double[MSIZE][MSIZE]();
  auto b =  new double[MSIZE][MSIZE]();
  auto c =  new double[MSIZE][MSIZE]();
  for (int i = 0; i < MSIZE; i++) {
    for (int j = 0; j < MSIZE; j++) {
      a[i][j] = (double)(i+1);
      b[i][j] = (double)(j+1);
    }
  }*/
  /* Running the implementation on test data */
/*  print_matrix->run<int>(MSIZE,c,"c");
  print_matrix->run<int>(MSIZE,a,"a"); 
  print_matrix->run<int>(MSIZE,b,"b"); 
  imp->run<double>(MSIZE,a, b,c);
  print_matrix->run<int>(MSIZE,c,"c"); 


 */
/* creating a performance model from the implementation */
//  double alpha = args.alpha->getValue();
//  uint64_t it = args.it->getValue();
//  int feat = args.feat->getValue();
//  PerfModel perf(imp);


/* Passing in a lambda which times the perf model on test data */
/*  perf.CreateModel(500,[&](const int size) -> double {
                    //create some random test data
                    std::default_random_engine gen;
                    std::uniform_real_distribution<double> dist(0.0,10000.0);
//                    SimpleTimer data_t;
//                    data_t.start();
                    //start timer
                     if (size == 0)
                      return 0.0;
                     auto a1 = (double*) malloc(size*size*sizeof(double));
                     auto b1 = (double*) malloc(size*size*sizeof(double));
                     auto c1 = (double*) malloc(size*size*sizeof(double));
                     for (int i = 0; i < size; i++) {
                       for (int j = 0; j < size; j++) {
                         a1[i*size + j] = dist(gen);
                         b1[i*size + j] = dist(gen);
                       }
                     }              
  //                   long datams = data_t.end();      
    //                 std::cout << "Generating data for input size[" << size 
      //                         << "] took: " << datams << "ms\n";
//                     boost::timer::cpu_timer test_t2;
                     SimpleTimer test_t;
                     test_t.start();
                     perf.imp->run<double>(size,a1,b1,c1);
  //                   test_t.stop();
                     uint64_t ms = test_t.end();
//                     boost::timer::cpu_times cput = test_t.elapsed(); 
                     std::stringstream ss;
                     ss << ms;
  //                   ss << format(cput,3,"%w");
                     delete a1;
                     delete b1;
                     delete c1;
                    //end timer
                    double msd;
                    ss >> msd;
//                    ms = cput.wall / 1000000.0;
                    return msd;
                  });
  std::cout << perf << "\n";
*/
/* dumping out the perf model */
 /* for ( int i = 0; i < 550; i+= 20) {
    std::cout << "Projected Cost of [" <<  i  << "] : ";
    fflush(stdout);
    std::cout << perf.QueryModel(i) << "\n";

  }
  perf.SaveToDisk();
  PerfModel p(imp);
  p.LoadFromDisk(p);
  std::cout << p << "\n";
  std::cout << "Comparing for Equality: ";
  if (p == perf) {
    std::cout << "SUCCESS\n";
  } else {
    std::cout << "FAIL\n";
  }*/

  e.start();
//  delete a;
//  delete b;
//  delete c;
//  delete print_matrix;
//  delete imp;
  return 0;
}

  
//  Resource res("DFE1",context);
//  res.Run("mmult");
//  res.getTopology();



//  Executor e;
//




/* Adding tasks */
/*  e.AddTask(new Task("MatrixMultiply"));
  Task *t = e.FindTask("MatrixMultiply");	

  if (t) {
    std::cout << *t << std::endl;
  } else {
    std::cout << "Couldn't find task\n";
  }
*/







