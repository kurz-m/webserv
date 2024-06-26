#include <ctime>
#include <sstream>
#include <unistd.h>

#include "EventLogger.hpp"

std::ofstream EventLogger::log_file_;
bool EventLogger::file_open_;
EventLogger::log_e EventLogger::log_level_;

// #define RESET "\033[0m"
// #define RED "\033[1;31m"
// #define YELLOW "\033[1;33m"
// #define BLUE "\033[1;34m"
// #define MAGENTA "\033[1;35m"
// #define CYAN "\033[1;36m"
// #define GREEN "\033[1;32m"

#define LOG_FILE "webserv.log"

EventLogger::EventLogger() {}

EventLogger::EventLogger(const std::string &filename) {
  file_open_ = false;
  log_level_ = log_e(__LOG_LEVEL__);
  std::ostringstream oss;
  std::string f_name = filename.substr(0, filename.find('.'));
  std::string f_ext = filename.substr(filename.find('.'));
  oss << f_name;
  if (access((oss.str() + f_ext).c_str(), F_OK) == 0) {
    int i = 1;
    oss << "_" << i;
    while (access((oss.str() + f_ext).c_str(), F_OK) == 0) {
      i++;
      oss.str("");
      oss.clear();
      oss << f_name + "_" << i;
    }
  }
  log_file_.open((oss.str() + f_ext).c_str());
  if (log_file_.is_open()) {
    file_open_ = true;
  }
}

void EventLogger::log(const std::string &fmt, const int line, const char *file,
                      log_e level) {
  if (file_open_) {
    std::time_t now = std::time(NULL);
    struct tm time_info;
    localtime_r(&now, &time_info);
    char m_time[70] = {0};
    strftime(m_time, sizeof(m_time), "%Y-%m-%d %T - ", &time_info);
    if (level >= log_level_) {
      switch (level) {
      case DEBUG:
        log_file_ << "[DEBUG] " << m_time << file << ":" << line << " - " + fmt
                  << std::endl;
        break;
      case INFO:
        log_file_ << "[INFO] " << m_time << file << ":" << line << " - " + fmt
                  << std::endl;
        break;
      case WARNING:
        log_file_ << "[WARNING] " << m_time << file << ":" << line
                  << " - " + fmt << std::endl;
        break;
      case ERROR:
        log_file_ << "[ERROR] " << m_time << file << ":" << line << " - " + fmt
                  << std::endl;
        break;
      }
    }
  }
}

EventLogger::~EventLogger() {}
