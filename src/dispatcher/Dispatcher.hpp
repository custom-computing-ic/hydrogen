#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

class Dispatcher {

  void mainServerLoop(int sockfd);

public:
  void movingAverage();

  void start(int port);

};


#endif /* _DISPATCHER_H_ */
