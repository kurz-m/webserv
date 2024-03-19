#ifndef __EVENT_LOGGER_HPP__
#define __EVENT_LOGGER_HPP__

#include <fstream>
#include <string>

enum log_e { TEST, INFO, DEBUG, ERROR };

class EventLogger {
public:
  EventLogger();
  EventLogger(const char *filename);

  void log(const std::string &message, const char *line, const char *file,
           log_e level);

private:
  ~EventLogger();

  static std::ofstream log_file_;
  static bool file_open_;
};

#endif
