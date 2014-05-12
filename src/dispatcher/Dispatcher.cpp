#include <Dispatcher.hpp>
#include <message.hpp>

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

void Dispatcher::movingAverage() {
  cout << "Dispatcher::MovingAverage" << endl;
}

void Dispatcher::handleRequest(msg_t& message) {
  cout << "Handling request " << endl;
  movingAverage();
}
