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
enum class windowName { UPTIME, TITLE, WAIT_TIME, SERVICE_TIME, NO_JOBS, UTIL,
                        THROUGHPUT, ARR_RATE, FREE_RES, TOT_RES, READYQTITLE,
                        RUNQTITLE,FINQTITLE,READYQ,RUNQ,FINQ,FREESPACE};
class StatsScreen {
  private:
    boost::mutex screenMtx;
    std::map<windowName,WINDOW*> windows;
    std::map<windowName,std::string> fieldMap;
    boost::signal<void(StatsScreen&)> resizeSignal;
    windowName currentWindow;
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
      /* Id, Foreground, Background */
      init_pair(1, COLOR_BLACK, COLOR_WHITE);
      init_pair(2, COLOR_WHITE, COLOR_BLACK);
      /* Set Correct Color */
      switch(os.currentWindow) {
        case windowName::TITLE:
        case windowName::UPTIME:
        case windowName::READYQTITLE:
        case windowName::RUNQTITLE:
        case windowName::FINQTITLE:
          wattron(os.windows[os.currentWindow],COLOR_PAIR(1));
          break;
        defaut:
          wattron(os.windows[os.currentWindow],COLOR_PAIR(2));
          break;
      }
      std::string field = os.fieldMap[os.currentWindow];
      std::string space = "";
      std::string emptychars = "";
      std::string remainder = "";

      switch(os.currentWindow) {
        case windowName::TITLE:
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
          break;

        case windowName::UPTIME:
          for (int i = 0; i < UPTIMEWIDTH - field.length() - obj.length() + 2; i++) {
            space += " ";
          }
          waddstr(os.windows[os.currentWindow],(field+ ": " + obj + space).c_str());
          break;

        case windowName::READYQTITLE:
        case windowName::RUNQTITLE:
        case windowName::FINQTITLE:
          for (int i = 0; i < os.width - field.length() - obj.length() + 2; i++) {
            space += " ";
          }
        case windowName::READYQ:
        case windowName::RUNQ:
        case windowName::FINQ:
          waddstr(os.windows[os.currentWindow],(field+ "" + obj + space).c_str());
          break;

        default:
          waddstr(os.windows[os.currentWindow],(field+ ": " + obj + space).c_str());
          break;
      }




      /* Draw the window */
      wrefresh(os.windows[os.currentWindow]);
      return os;
    }

    friend StatsScreen& operator<< (StatsScreen& os, const windowName& obj){
      boost::lock_guard<boost::mutex> guard(os.screenMtx);
      switch(obj){
        case windowName::UTIL:
        case windowName::THROUGHPUT:
        case windowName::WAIT_TIME:
        case windowName::SERVICE_TIME:
        case windowName::NO_JOBS:
        case windowName::ARR_RATE:
        case windowName::TITLE:
        case windowName::UPTIME:
        case windowName::FREE_RES:
        case windowName::TOT_RES:
        case windowName::RUNQTITLE:
        case windowName::READYQTITLE:
        case windowName::FINQTITLE:
        case windowName::RUNQ:
        case windowName::READYQ:
        case windowName::FINQ:
          os.currentWindow = obj;
          break;
        default:
          os.currentWindow = windowName::FREESPACE;
        }
        return os;
      }
};
#endif
