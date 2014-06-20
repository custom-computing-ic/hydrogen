#include <StatsScreen.hpp>
sig_atomic_t resizeFlag;
StatsScreen::StatsScreen()  {
  boost::lock_guard<boost::mutex> guard(this->screenMtx);
  std::cout << "StatusScreen()" << std::endl;
  currentWindow = 0;
  resizeFlag = 0;
  initscr();
  nonl();
  cbreak();
  if (has_colors() == FALSE) {

  }
  echo();
  maxX=0;
  maxY=0;
  this->setupWindows();
  signal(SIGWINCH,StatsScreen::handle_WINCH);
  std::cout << "Screen Width: " << maxX << "\tScreen Height: " << maxY;
  fieldMap[TITLE]        = "TITLE";
  fieldMap[UTIL]         = "UTIL";
  fieldMap[THROUGHPUT]   = "THROUGHPUT";
  fieldMap[WAIT_TIME]    = "WAIT_TIME";
  fieldMap[SERVICE_TIME] = "SERVICE_TIME";
  fieldMap[NO_JOBS]      = "NO_JOBS";
  fieldMap[ARR_RATE]     = "ARRIVAL_RATE";
  resizeSignal.connect(boost::bind(&StatsScreen::resize,_1));
}

StatsScreen::~StatsScreen() {
  this->delWindows();
  endwin();
  std::cout << "~StatusScreen()" << std::endl;
  std::cout << "Screen Width: " << maxX << "\tScreen Height: " << maxY;
}
void StatsScreen::refreshScreen() {
  refresh();
  std::vector<WINDOW*>::iterator w;
  for (w = windows.begin(); w!= windows.end(); w++) {
    wrefresh(*w);
  }
}

void StatsScreen::delWindows() {
  while (windows.size() > 0) {
    WINDOW* w = windows.back();
    windows.pop_back();
    delwin(w);
  }
  endwin();
  this->refreshScreen();
}
void StatsScreen::setupWindows() {
  getmaxyx(stdscr,maxY,maxX);
  lines = 1;
  width = 25;
  windows.push_back(newwin(1,maxX,0,0));
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
//    resizeSignal(*this);
      resize(*this);
  }
}

void StatsScreen::resize(StatsScreen& s) {
  boost::unique_lock<boost::mutex> lock(s.screenMtx);
  resizeFlag = 0;
  s.delWindows();
  s.setupWindows();
  lock.unlock();
  std::string title =  "Resized (" +  std::to_string(s.maxX) + "," + std::to_string(s.maxY)
     + ")";
//  s  << TITLE <<  title;
}
