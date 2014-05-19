#ifndef __SIMPLE_TIMER_CLASS_H__
#define __SIMPLE_TIMER_CLASS_H__
#include <chrono>
#include <utility>

typedef std::chrono::time_point<std::chrono::system_clock> ticktock;



class SimpleTimer
{
private:
  ticktock c;
  
public:
  
  void start() {
    c = std::chrono::system_clock::now();
  }
  
  int end() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - c).count(); 
  }
  
  
};

#endif
