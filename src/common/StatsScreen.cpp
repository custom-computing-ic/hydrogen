#include <StatsScreen.hpp>

StatsScreen::StatsScreen() {
  std::cout << "StatusScreen()" << std::endl;
  currentWindow = 0;
//  (void) signal(SIGINT,finish);
  initscr();
  nonl();
  cbreak();
  echo();
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 80; i+=30) {
      windows.push_back(newwin(LINES,WIDTH,j,i));
    }
  }
  fieldMap[UTIL] = "UTIL";
  fieldMap[THROUGHPUT] = "THROUGHPUT";
  fieldMap[WAIT_TIME] = "WAIT_TIME";
  fieldMap[SERVICE_TIME] = "SERVICE_TIME";
  fieldMap[NO_JOBS] = "NO_JOBS";
  fieldMap[ARR_RATE] = "ARRIVAL_RATE";
}

StatsScreen::~StatsScreen() {
  std::vector<WINDOW*>::iterator w;
  for (w = windows.begin(); w!= windows.end(); w++) {
    delwin(*w);
  }
  endwin();
  std::cout << "~StatusScreen()" << std::endl;
}
void StatsScreen::refresh() {
  std::vector<WINDOW*>::iterator w;
  for (w = windows.begin(); w!= windows.end(); w++) {
    wrefresh(*w);
  }
}
