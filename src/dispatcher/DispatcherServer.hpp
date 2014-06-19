#ifndef _DISPATCHERSERVER_H_
#define _DISPATCHERSERVER_H_

#include <MultiThreadedTCPServer.hpp>
#include <message.hpp>
#include <string>

#ifdef USEDFE
#include <DfeLib.hpp>
#endif

class DispatcherServer : public MultiThreadedTCPServer {

  bool useDfe;
  bool useGroups;
  int nDfes;
public:

  DispatcherServer(const std::string& address,
                   const std::string& name,
                   std::size_t thread_pool_size, bool usGps,
                   int nDFE) :
    super(address, name, thread_pool_size)
  {

#ifdef USEDFE
    std::cout << "Starting Dispatcher Server using DFE implementations" << std::endl;
    useDfe = true;
#else
    std::cout << "Starting Dispatcher Server without DFE implementations" << std::endl;
    useDfe = false;
#endif
    useGroups = usGps;
    std::cout << "Default # DFE's to run Jobs with RID[0000] on: "
              << nDFE;
    this->nDfes = nDFE;
    if (useGroups)
      std::cout << "Using group DFE loading interface" << std::endl;
    else
      std::cout << "Using custom OpenMP DFE loading interface" << std::endl;
  }

  void movingAverage_cpu(size_t n, size_t size, int *data, int *out);
  void movingAverage_dfe(int n, int size, int *data, int *out, bool useGroups);
  void movingAverage_dfe(int n, int size, int *data, int *out,
                         char** dfeIds, int nDfes, bool useGroups);
  void optionPricing_dfe(double strike,
			 double sigma,
			 double timestep,
			 int numMaturity,
			 int paraNode,
			 int numPathGroup,
			 double T,
			 double *out
			 );

  msg_t* handle_request(msg_t* request);
};


#endif /* _DISPATCHERSERVER_H_ */
