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
enum windowName {UPTIME, TITLE, WAIT_TIME, SERVICE_TIME, NO_JOBS, UTIL, THROUGHPUT, ARR_RATE};
class StatsScreen {
  private:
    boost::mutex screenMtx;
    std::vector<WINDOW*> windows;
    std::map<int,std::string> fieldMap;
    boost::signal<void(StatsScreen&)> resizeSignal;
    int currentWindow;
    static const int UPTIMEWIDTH = 15;
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
      wclear(os.windows[os.currentWindow]);
      wrefresh(os.windows[os.currentWindow]);
      start_color();
      init_pair(1, COLOR_BLACK, COLOR_WHITE);
      init_pair(2, COLOR_WHITE, COLOR_BLACK);
      /* Set Correct Color */
      if (os.currentWindow != TITLE && os.currentWindow != UPTIME) {
        wattron(os.windows[os.currentWindow],COLOR_PAIR(2));
      } else {
        wattron(os.windows[os.currentWindow],COLOR_PAIR(1));
      }
      /* Deal with uptime as a special case */
      if (os.currentWindow != TITLE) {
        std::string field = os.fieldMap[os.currentWindow];
        std::string space = "";
        if (os.currentWindow == UPTIME) {
          for (int i = 0; i < UPTIMEWIDTH - field.length() - obj.length() + 2; i++) {
            space += " ";
          }
        }
        waddstr(os.windows[os.currentWindow],(field+ ": " + obj + space).c_str());
      } else {
        std::string emptychars = "";
        std::string remainder = "";
        /* Center the title ... */
        for (int i = 0; i < (os.maxX - obj.length() - 2*UPTIMEWIDTH)/2; i++) {
          emptychars+=" ";
        }
        for (int i = 0; i <  (os.maxX - obj.length() - 2*UPTIMEWIDTH)%2; i++) {
          remainder += " ";
        }
        for (int i = 0; i < UPTIMEWIDTH; i++) {
          remainder += " ";
        }
        mvwaddstr(os.windows[os.currentWindow],0,0,
            (emptychars + obj + emptychars + remainder).c_str());
      }
      /* Draw the window */
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
        case UPTIME:
          os.currentWindow = obj;
          break;
        default:
          os.currentWindow = 0;
        }
        return os;
      }
};
#endif
