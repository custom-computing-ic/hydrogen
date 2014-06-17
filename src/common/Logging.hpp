#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <string>

#include <boost/format.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>


void initLogging(std::string logFile);

std::string parse(std::string);

#define LOG(level) BOOST_LOG_TRIVIAL(level) << " " << parse(BOOST_CURRENT_FUNCTION) << ":"
#define LOGF(level, msg) BOOST_LOG_TRIVIAL(level) << " " << parse(BOOST_CURRENT_FUNCTION) << ":" << boost::format(msg)

#endif /* _LOGGING_H_ */
