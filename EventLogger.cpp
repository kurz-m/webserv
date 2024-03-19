#include "EventLogger.hpp"

#define LOG_FILE "webserv.log"

EventLogger::EventLogger() {}

EventLogger::EventLogger(const char *filename) {
  log_file_.open(filename, std::ios::app);
  if (log_file_.is_open()) {
    file_open_ = true;
  }
}

void EventLogger::log(const std::string &message, const char *line,
                      const char *file, log_e level) {}
