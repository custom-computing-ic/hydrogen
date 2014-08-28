#include "Logging.hpp"

#include <iostream>

std::string parse(std::string functionName) {

  // this will remove the type signature

  // remove arguments
  size_t pos = functionName.find("(");
  functionName.erase(pos, functionName.size() - pos);
  pos = functionName.rfind(" ");

  // remove return type
  if (pos != std::string::npos) {
    return functionName.substr(pos, functionName.size() - pos);
  }

  return functionName;
}

void initLogging(std::string logFile) {
  namespace expr = boost::log::expressions;
  namespace keywords = boost::log::keywords;
  namespace logging = boost::log;
  namespace attrs = boost::log::attributes;

  // TODO[paul-g] enabling both file and console logging seems to cause a segfault...
  // boost::log::add_file_log
  //   (
  //    logFile,
  //    keywords::format = expr::stream
  //    << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
  //    << "[" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
  //    << "] >> " << expr::message
  //   );

  logging::add_common_attributes();
  logging::add_console_log(std::cout,
			   keywords::format = expr::stream
			   << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S")
			   << expr::message);
}
