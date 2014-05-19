#ifndef __UTILS_H__
#define __UTILS_H__
#include <iostream>
#include <tclap/CmdLine.h>

/* Command line data structure */
typedef struct cmd_args {
  TCLAP::ValueArg<double> *alpha;
  TCLAP::ValueArg<uint64_t> *it;
  TCLAP::ValueArg<int> *feat;
} cmd_args_t;
/* Command line parser */
int parse_cmd(int argc, char **argv, cmd_args_t *a);
/* String printing functions */
std::string prd(const double x, const int decDigits, const int width);
std::string prd(const double x, const int width);
std::string centre(const std::string s, const int w);
std::string prsR(const std::string s, const int w);


#endif
