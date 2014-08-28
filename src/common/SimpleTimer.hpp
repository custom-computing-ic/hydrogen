#ifndef __SIMPLE_TIMER_CLASS_H__
#define __SIMPLE_TIMER_CLASS_H__
//#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <utility>

using namespace boost::posix_time;
class SimpleTimer {
private:
  /*  typedef std::chrono::milliseconds Milliseconds;
    typedef std::chrono::system_clock sysclock;
    typedef std::chrono::steady_clock sysclock;
    typedef std::chrono::time_point<sysclock> timePoint;
    timePoint c;*/

  ptime c;

public:
  void start() { c = microsec_clock::universal_time(); }

  long end() {
    return (microsec_clock::universal_time() - c).total_milliseconds();
  }
};
#endif
