#include <iostream>
#include <functional>

#include <utils.hpp>
//#include "timer/timer.h"
//#include "timer/simpleTimer.h"
#include <SimpleTimer.hpp>
#include <Executor.hpp>
#include <Implementation.hpp>
//#include "build/gen/control.pb.h"
#define MSIZE 2


int main(int argc, char** argv) {
  cmd_args_t args;
  parse_cmd(argc,argv,&args); 

  Executor e(args.execPort->getValue(),"localhost",args.schedPort->getValue(),args.hostName->getValue());
  /*loading implementations from libraries */
  Implementation *imp = new Implementation("./libMM.so","mmult");
  /* Checking for error loading the library */
  if (imp->load() == NULL) {
    std::cout << "Exiting" << std::endl;
    return 1;
  }
  Implementation *print_matrix = new Implementation("./libMM.so","print_matrix");
  if (print_matrix->load() == NULL) {
    std::cout << "Exiting" << std::endl;
    return 1;
  }



  auto a =  new double[MSIZE][MSIZE]();
  auto b =  new double[MSIZE][MSIZE]();
  auto c =  new double[MSIZE][MSIZE]();
  for (int i = 0; i < MSIZE; i++) {
    for (int j = 0; j < MSIZE; j++) {
      a[i][j] = (double)(i+1);
      b[i][j] = (double)(j+1);
    }
  }
  /* Running the implementation on test data */
  print_matrix->run<int>(MSIZE,c,"c");
  print_matrix->run<int>(MSIZE,a,"a"); 
  print_matrix->run<int>(MSIZE,b,"b"); 
  imp->run<double>(MSIZE,a, b,c);
  print_matrix->run<int>(MSIZE,c,"c"); 


 
/* creating a performance model from the implementation */
  double alpha = args.alpha->getValue();
  uint64_t it = args.it->getValue();
  int feat = args.feat->getValue();
  PerfModel perf(imp,alpha,it,feat,3);


/* Passing in a lambda which times the perf model on test data */
  perf.CreateModel(500,[&](const int size) -> double {
                    //start timer
                     SimpleTimer test_t;
                     if (size == 0)
                      return 0.0;
                     auto a1 = (double*) malloc(size*size*sizeof(double));
                     auto b1 = (double*) malloc(size*size*sizeof(double));
                     auto c1 = (double*) malloc(size*size*sizeof(double));
                     for (int i = 0; i < size; i++) {
                       for (int j = 0; j < size; j++) {
                         a1[i*size + j] = (double)(i+1);
                         b1[i*size + j] = (double)(j+1);
                       }
                     }                    
                     test_t.start();
                     perf.imp->run<double>(size,a1,b1,c1);
                     int ms = test_t.end();
                     delete a1;
                     delete b1;
                     delete c1;
                    //end timer

                    return ms;
                  });
  perf.SaveToDisk();
  delete a;
  delete b;
  delete c;
  delete print_matrix;
  delete imp;
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





/* dumping out the perf model */
/*  for ( int i = 0; i < 550; i+= 20) {
    std::cout << "Projected Cost of [" <<  i  << "] : ";
    fflush(stdout);
    std::cout << perf.QueryModel(i) << "\n";

  }*/

