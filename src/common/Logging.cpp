#include "Logging.hpp"

void initLogging(std::string logFile) {
  namespace expr = boost::log::expressions;
  namespace keywords = boost::log::keywords;
  namespace logging = boost::log;
  namespace attrs = boost::log::attributes;

  boost::log::add_file_log
    (
     logFile,
     keywords::format = expr::stream
     << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
     << "[" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
     << "] >> " << expr::message
    );

  logging::add_common_attributes();
  logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());
}
