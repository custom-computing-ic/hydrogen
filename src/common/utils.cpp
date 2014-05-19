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
  std:: stringstream ss, spaces;
  int padding = w - s.size();
  for (int i=0; i<padding; ++i)
    spaces << " ";
  ss << spaces.str() << s;
  return ss.str();
}



/*
 * Command line parsing 
 */
int parse_cmd(int argc, char **argv, cmd_args_t *a) {
  try {
   TCLAP::CmdLine cmd("Program description message",' ', "1,0");
 
   a->alpha = new TCLAP::ValueArg<double> ("a",
                                           "alpha",
                                           "learning rate",
                                           false,
                                           0,
                                           "double",
                                           cmd);

   a->it = new TCLAP::ValueArg<uint64_t>("i",
                                      "num_iterations",
                                      "Number of iterations for linear regression",
                                      false,
                                      1e6,
                                      "uint64_t",
                                      cmd);

   a->feat = new TCLAP::ValueArg<int>("f",
                                      "no_features",
                                      "Number of features to add",
                                      false,
                                      4,
                                      "int",
                                      cmd);
   cmd.parse(argc,argv);

  } catch(TCLAP::ArgException &e) {
    std::cout << "Error[" << e.argId() << "]: " << e.error() << std::endl;
    return 1;
  }
  return 0;
}


