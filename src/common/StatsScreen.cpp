#include <StatsScreen.hpp>
sig_atomic_t resizeFlag;
StatsScreen::StatsScreen()  {
  boost::lock_guard<boost::mutex> guard(this->screenMtx);
//  std::cout << "StatusScreen()" << std::endl;
  currentWindow = windowName::FREESPACE;
  resizeFlag = 0;
  initscr();
  nonl();
  cbreak();
  curs_set(0);
  if (has_colors() == FALSE) {

  }
  echo();
  maxX=0;
  maxY=0;
  width = 0;
  /* separate into fieldMap and Title Map? */
  fieldMap[windowName::UPTIME]       = "Uptime";
  fieldMap[windowName::TITLE]        = "TITLE";
  fieldMap[windowName::UTIL]         = "Cluster Utilization";
  fieldMap[windowName::THROUGHPUT]   = "Throughput";
  fieldMap[windowName::WAIT_TIME]    = "Avg. Wait Time";
  fieldMap[windowName::SERVICE_TIME] = "Avg. Service Time";
  fieldMap[windowName::NO_JOBS]      = "Job Completions";
  fieldMap[windowName::ARR_RATE]     = "Arrival Rate";
  fieldMap[windowName::FREE_RES]     = "Free Resources";
  fieldMap[windowName::TOT_RES]      = "Total Resources";
  fieldMap[windowName::READYQTITLE] = "ReadyQ";
  fieldMap[windowName::RUNQTITLE]   = "RunQ";
  fieldMap[windowName::FINQTITLE]   = "FinishedQ";
  fieldMap[windowName::READYQ] = " ";
  fieldMap[windowName::RUNQ]   = " ";
  fieldMap[windowName::FINQ]   = " ";

  for (const auto& elem : fieldMap) {
    width = elem.second.length() > width ? elem.second.length() : width;
   }
  width += 18;
  setupWindows();
  signal(SIGWINCH,StatsScreen::handle_WINCH);
 // std::cout << "Screen Width: " << maxX << "\tScreen Height: " << maxY;
}

StatsScreen::~StatsScreen() {
  this->delWindows();
  endwin();
//  std::cout << "~StatusScreen()" << std::endl;
//  std::cout << "Screen Width: " << maxX << "\tScreen Height: " << maxY;
}
void StatsScreen::refreshScreen() {
  refresh();
  clear();
  std::map<windowName,WINDOW*>::iterator w;
  for (w = windows.begin(); w!= windows.end(); w++) {
    wrefresh(w->second);
    wclear(w->second);
  }
}

void StatsScreen::delWindows() {
  while (windows.size() > 0) {
    WINDOW* w = windows.begin()->second;
    windows.erase(windows.begin());
    delwin(w);
  }
  endwin();
}
void StatsScreen::setupWindows() {
  getmaxyx(stdscr,maxY,maxX);
  lines = 1;
  /* 15 chars for the uptime string */
  windows[windowName::UPTIME] = newwin(lines,UPTIMEWIDTH,0,0);
  /* Title block */
  windows[windowName::TITLE] = newwin(lines,maxX-UPTIMEWIDTH,0,15);
  /* Scheduler Stats */
  int noFields = fieldMap.size() - 2;
  int i = 0;
  int row = 0;
  int count = 2;

  for (row = 1; count < noFields; row++) {
    for (i = 0; i < maxX-width; i +=  width ) {
      /* Ugly cast.. */
      windows[static_cast<windowName>(count)] = newwin(lines,width,row,i);
      count++;
    }
    /* Small fudge for loop checking... I should really figure out a better way */
  }
  /* Setup Stuff for Q information */
  windows[windowName::READYQTITLE] =  newwin(lines,width,row,0);
  windows[windowName::RUNQTITLE] =  newwin(lines,width,row,width);
  windows[windowName::FINQTITLE] =  newwin(lines,width,row,width*2);
  row++;
  windows[windowName::READYQ] =  newwin(maxY-row,width,row,0);
  windows[windowName::RUNQ] =  newwin(maxY-row,width,row,width);
  windows[windowName::FINQ] =  newwin(maxY-row,width,row,width*2);


}
void StatsScreen::handle_WINCH(int sig) {
  resizeFlag = 1;
}
void StatsScreen::check_resize() {
  if (resizeFlag) {
      resize(*this);
  }
}

void StatsScreen::resize(StatsScreen& s) {
  boost::unique_lock<boost::mutex> lock(s.screenMtx);
  resizeFlag = 0;
  s.delWindows();
  s.refreshScreen();
  s.setupWindows();
  s.refreshScreen();
  lock.unlock();
//  std::string title =  "Resized (" +  std::to_string(s.maxX) + "," + std::to_string(s.maxY)
//     + ")";
//  s  << TITLE <<  title;
}
