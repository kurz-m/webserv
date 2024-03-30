#ifndef __EVENT_LOGGER_HPP
#define __EVENT_LOGGER_HPP

#ifndef __LOG_LEVEL__
#define __LOG_LEVEL__ INFO
#endif

#define LOG_DEBUG(X)                                                           \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, DEBUG);                                  \
  } while (0)
#define LOG_INFO(X)                                                            \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, INFO);                                   \
  } while (0)
#define LOG_WARNING(X)                                                         \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, WARNING);                                \
  } while (0)
#define LOG_ERROR(X)                                                           \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, ERROR);                                  \
  } while (0)

#include <fstream>
#include <string>

enum log_e {
  DEBUG,   /**<Level 0: Debug information in development*/
  INFO,    /**<Level 1: General information*/
  WARNING, /**<Level 2: Warning messages for non-critical events*/
  ERROR,   /**<Level 3: Error messages for error in setup*/
};

class EventLogger {
public:
  EventLogger();
  EventLogger(const std::string &filename);
  ~EventLogger();

  void log(const std::string &fmt, const int line, const char *file,
           log_e level);

private:
  static std::ofstream log_file_;
  static bool file_open_;
  static log_e log_level_;
};

#endif /* EventLogger.hpp */
