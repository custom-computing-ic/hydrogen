#ifndef _CLIENT_API_H_
#define _CLIENT_API_H_

#include <cstddef>
#include <string>
/** Contains client stubs. */

void movingAverage(std::size_t n, std::size_t size, int* data, int* out);
/** Version of Moving average where client can specify port # */
void movingAverage(size_t n, size_t size, int* data, int* out, int pNum);

void addResource(std::string type, std::string host, std::string port, int clientId);


#endif /* _CLIENT_H_ */
