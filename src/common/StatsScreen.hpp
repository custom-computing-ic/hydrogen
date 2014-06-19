#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#define LINES 1
#define WIDTH 20
extern sig_atomic_t stopFlag;
/* A simple NCurses based gui for presentation
 *
 * -  Only insert strings to the screen i.e.
 *    statusScreen << somestringobjhere
 *
 *  - If you need to print integer data, construct a string first
 *  - It uses int's to decide which screen area to write to.
 */

class StatsScreen;
enum windowName { WAIT_TIME, SERVICE_TIME, NO_JOBS, UTIL, THROUGHPUT, ARR_RATE};
class StatsScreen {
  private:
    std::vector<WINDOW*> windows;
    std::map<int,std::string> fieldMap;
    int currentWindow;
  public:
    StatsScreen();
    ~StatsScreen();
    void refresh();
    static void finish(int sig) {
        stopFlag = 1;
    };
    friend StatsScreen& operator<< (StatsScreen& os, const std::string& obj){
      werase(os.windows[os.currentWindow]);
      std::string field = os.fieldMap[os.currentWindow];
      waddstr(os.windows[os.currentWindow],(field+ ": " + obj).c_str());
      wrefresh(os.windows[os.currentWindow]);
      return os;
    }
    friend StatsScreen& operator<< (StatsScreen& os, const int& obj){
      switch(obj){
        case UTIL:
        case THROUGHPUT:
        case WAIT_TIME:
        case SERVICE_TIME:
        case NO_JOBS:
        case ARR_RATE:
          os.currentWindow = obj;
          break;
        default:
          os.currentWindow = 0;
        }
        return os;
      }
};
