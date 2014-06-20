#include <StatsScreen.hpp>
sig_atomic_t resizeFlag;
StatsScreen::StatsScreen()  {
  boost::lock_guard<boost::mutex> guard(this->screenMtx);
//  std::cout << "StatusScreen()" << std::endl;
  currentWindow = 0;
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
  fieldMap[UPTIME]       = "Uptime";
  fieldMap[TITLE]        = "TITLE";
  fieldMap[UTIL]         = "UTIL";
  fieldMap[THROUGHPUT]   = "THROUGHPUT";
  fieldMap[WAIT_TIME]    = "WAIT_TIME";
  fieldMap[SERVICE_TIME] = "SERVICE_TIME";
  fieldMap[NO_JOBS]      = "NO_JOBS";
  fieldMap[ARR_RATE]     = "ARRIVAL_RATE";
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
  std::vector<WINDOW*>::iterator w;
  for (w = windows.begin(); w!= windows.end(); w++) {
    wrefresh(*w);
    wclear(*w);
  }
}

void StatsScreen::delWindows() {
  while (windows.size() > 0) {
    WINDOW* w = windows.back();
    windows.pop_back();
    delwin(w);
  }
  endwin();
}
void StatsScreen::setupWindows() {
  getmaxyx(stdscr,maxY,maxX);
  lines = 1;
  /* 15 chars for the uptime string */
  windows.push_back(newwin(lines,UPTIMEWIDTH,0,0));
  /* Title block */
  windows.push_back(newwin(lines,maxX-UPTIMEWIDTH,0,15));

  for (int j = 1; j < 4; j++) {
    for (int i = 0; i < maxX-width; i +=  width ) {
      windows.push_back(newwin(lines,width,j,i));
    }
  }


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
