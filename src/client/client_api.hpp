#ifndef _CLIENT_API_H_
#define _CLIENT_API_H_

#include <cstddef>
#include <string>
/** Contains client stubs. */

void movingAverage(std::size_t n, std::size_t size, int* data, int* out);
void optionPricing(double strike,
		   double sigma,
		   double timestep,
		   int numMaturity,
		   int paraNode,
		   int numPathGroup,
		   double T,
		   double *out);

void TerminateServer();
/** Version of Moving average where client can specify port # */
void movingAverage(size_t n, size_t size, int* data, int* out, int pNum);

void addResource(std::string type, std::string host, std::string port, int clientId);


#endif /* _CLIENT_H_ */
