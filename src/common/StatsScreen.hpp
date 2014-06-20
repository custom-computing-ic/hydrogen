#ifndef __STATS_SCREEN__
#define __STATS_SCREEN__
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <boost/bind.hpp>
#include <boost/signal.hpp>
#include <boost/thread.hpp>
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
enum windowName { TITLE, WAIT_TIME, SERVICE_TIME, NO_JOBS, UTIL, THROUGHPUT, ARR_RATE};
class StatsScreen {
  private:
    boost::mutex screenMtx;
    std::vector<WINDOW*> windows;
    std::map<int,std::string> fieldMap;
    boost::signal<void(StatsScreen&)> resizeSignal;
    int currentWindow;
    int maxX;
    int maxY;
    int lines;
    int width;
  public:
    StatsScreen();
    ~StatsScreen();
    void refreshScreen();
    static void finish(int sig) {
        stopFlag = 1;
    };
    static void handle_WINCH(int);
    void check_resize();
    static void resize(StatsScreen&);
    void delWindows();
    void setupWindows();
    friend StatsScreen& operator<< (StatsScreen& os, const std::string& obj){
      boost::lock_guard<boost::mutex> guard(os.screenMtx);
      werase(os.windows[os.currentWindow]);
      start_color();
      init_pair(1, COLOR_BLACK, COLOR_WHITE);
      init_pair(2, COLOR_WHITE, COLOR_BLACK);

      if (os.currentWindow != TITLE) {
        wattron(os.windows[os.currentWindow],COLOR_PAIR(2));
        std::string field = os.fieldMap[os.currentWindow];
        waddstr(os.windows[os.currentWindow],(field+ ": " + obj).c_str());
      } else {
        wattron(os.windows[os.currentWindow],COLOR_PAIR(1));
        std::string emptychars = "";
        std::string remainder = "";
        for (int i = 0; i < (os.maxX - obj.length())/2; i++) {
          emptychars+=" ";
        }
        for (int i = 0; i <  (os.maxX - obj.length())%2; i++) {
          remainder += " ";
        }
        wclear(os.windows[os.currentWindow]);
        mvwaddstr(os.windows[os.currentWindow],0,0,
            (emptychars + obj + emptychars + remainder).c_str());
      }
      wrefresh(os.windows[os.currentWindow]);
      return os;
    }

    friend StatsScreen& operator<< (StatsScreen& os, const int& obj){
      boost::lock_guard<boost::mutex> guard(os.screenMtx);
      switch(obj){
        case UTIL:
        case THROUGHPUT:
        case WAIT_TIME:
        case SERVICE_TIME:
        case NO_JOBS:
        case ARR_RATE:
        case TITLE:
          os.currentWindow = obj;
          break;
        default:
          os.currentWindow = 0;
        }
        return os;
      }
};
#endif
