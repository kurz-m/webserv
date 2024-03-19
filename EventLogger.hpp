#ifndef __EVENT_LOGGER_HPP__
#define __EVENT_LOGGER_HPP__

#ifndef __LOG_LEVEL__
#define __LOG_LEVEL__ INFO
#endif

#include <fstream>
#include <string>

enum log_e { DEBUG, INFO, WARNING, ERROR };

class EventLogger {
public:
  EventLogger();
  EventLogger(const char *filename);
  ~EventLogger();

  void log(const std::string &fmt, const int line, const char *file,
           log_e level);

private:
  static std::ofstream log_file_;
  static bool file_open_;
  static log_e log_level_;
};

#endif
