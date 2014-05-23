#include <utils.hpp>
/* 
 * Pretty printing functions
 * Thanks to synaptik: http://goo.gl/AH0UKC
 */
std::string prd(const double x, const int decDigits, const int width) {
  std::stringstream ss;
  ss << std::fixed << std::right;
  ss.fill(' ');
  ss.width(width);
  ss.precision(decDigits);
  ss << x;
  return ss.str();

}
std::string prd(const double x, const int width) {
  std::stringstream ss;
  ss << std::fixed << std::right;
  ss.fill(' ');
  ss.width(width);
  ss << x;
  return ss.str();

}

std::string centre(const std::string s, const int w) {

  std::stringstream ss, spaces;
  int padding = w - s.size();
  for (int i = 0; i < padding/2; ++i)
      spaces << " ";
  ss << spaces.str() << s << spaces.str();
  if (padding >0 && padding%2!=0)
    ss << " " ;
  return ss.str();

}

std::string prsR(const std::string s, const int w) {
  std::stringstream ss, spaces;
  int padding = w - s.size();
  for (int i=0; i<padding; ++i)
    spaces << " ";
  ss << spaces.str() << s;
  return ss.str();
}



/*
 * Command line parsing 
 */



