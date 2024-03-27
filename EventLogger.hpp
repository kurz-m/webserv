#ifndef __EVENT_LOGGER_HPP__
#define __EVENT_LOGGER_HPP__

#ifndef __LOG_LEVEL__
#define __LOG_LEVEL__ INFO
#endif

#define LOG_DEBUG(X)                                                           \
  {                                                                            \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, DEBUG);                                  \
  }
#define LOG_INFO(X)                                                            \
  {                                                                            \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, INFO);                                   \
  }
#define LOG_WARNING(X)                                                         \
  {                                                                            \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, WARNING);                                \
  }
#define LOG_ERROR(X)                                                           \
  {                                                                            \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, ERROR);                                  \
  }

#include <fstream>
#include <string>

enum log_e { DEBUG, INFO, WARNING, ERROR };

class EventLogger {
public:
  EventLogger();
  EventLogger(const std::string& filename);
  ~EventLogger();

  void log(const std::string &fmt, const int line, const char *file,
           log_e level);

private:
  static std::ofstream log_file_;
  static bool file_open_;
  static log_e log_level_;
};

#endif
