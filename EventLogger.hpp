#ifndef __EVENT_LOGGER_HPP
#define __EVENT_LOGGER_HPP

#ifndef __LOG_LEVEL__
#define __LOG_LEVEL__ INFO
#endif

#define LOG_DEBUG(X)                                                           \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, EventLogger::DEBUG);                     \
  } while (0)
#define LOG_INFO(X)                                                            \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, EventLogger::INFO);                      \
  } while (0)
#define LOG_WARNING(X)                                                         \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, EventLogger::WARNING);                   \
  } while (0)
#define LOG_ERROR(X)                                                           \
  do {                                                                         \
    EventLogger logger;                                                        \
    logger.log(X, __LINE__, __FILE__, EventLogger::ERROR);                     \
  } while (0)

#include <fstream>
#include <string>

class EventLogger {
public:
  EventLogger();
  EventLogger(const std::string &filename);
  ~EventLogger();

  /**
   * Enum that defines the different log levels.
   *
   * The log level is defined during compilation time with the LOG_LEVEL=X
   * input. The default log level is 4, which means no logs will be generated.
   * For the other log leves it always defines which is the lowest log level
   * that will be printed
   */
  enum log_e {
    DEBUG,   /**<Level 0: Debug information in development*/
    INFO,    /**<Level 1: General information*/
    WARNING, /**<Level 2: Warning messages for non-critical events*/
    ERROR,   /**<Level 3: Error messages for error in setup*/
  };

  void log(const std::string &fmt, const int line, const char *file,
           log_e level);

private:
  static std::ofstream log_file_;
  static bool file_open_;
  static log_e log_level_;
};

#endif /* EventLogger.hpp */
