#include <Resource.hpp>
/*
 *
 */
msg_t* Resource::dispatch(msg_t* m) {
  if (m == NULL) {
    std::cout << "(ERROR): Recieved ((msg_t*)NULL) when trying to run "
              << "Implementation.\n";
    return (msg_t*)NULL;
  }
  int sizeBytes = sizeof(msg_t) + sizeof(int)*(m->dataSize);
  char* buff = (char*) calloc(sizeBytes,1);
  msg_t* rsp = (msg_t*) buff;
//  int iport;
//  std::stringstream ss(port);
//  ss >> iport;
  Client c (port,url);
  c.start();
  c.send(m);
  /* TODO[mtottenh]: Check this recv code against pauls implementation*/
  do {
    c.read(buff,sizeBytes);
  } while ( rsp->msgId != MSG_RESULT);
  c.stop();
  return rsp;
}
