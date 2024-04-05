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

/**
 * EventLogger logs events based on different levels.
 *
 * The EventLogger class logs the events with a specific log level to a file
 * with the name 'webserv*.log' where * represents a continous increasing
 * number. During the startup, the event logger creates a file with the
 * appropriate name as static member attribute which gives all instances of the
 * EventLogger the possibility to write to it. On specified events, the
 * EventLogger is invoked with the macro LOG_[LOG_LEVEL](message). This creates
 * a scope in which an instance is created, the event is being logged and the
 * instance is being destructed again.
 */
class EventLogger {
public:
  /**
   * Default constructor used for logging actions.
   *
   * The default constructor is being used to create instances of EventLogger
   * during the logging of an event. Because the file is a static private
   * member, all instances have access to it.
   */
  EventLogger();

  /**
   * Parameterized constructor for the EventLogger class.
   *
   * This parameterized constructor is being used during startup of the
   * webserver to open a file in which the events are being logged.
   */
  EventLogger(const std::string &filename);

  /**
   * Default destructor of the EventLogger class.
   *
   * No action needed, because the file.close() function is being called when
   * the lifetime of the EventLogger ends.
   */
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

  /**
   * Main log function for logging events of different log levels.
   *
   * The log function takes 4 arguments which are specifying the line and the
   * file where the log function is being called, the log level, and also a
   * string representation of the log message that should be printed.
   * Furthermore, it adds the timestamp to the message to be able to see when
   * the event occured.
   *
   * \param fmt Log message of the event.
   * \param line Line in the source file where log() is being called.
   * \param file Source file where log() is being called.
   * \param level Log level of the message.
   */
  void log(const std::string &fmt, const int line, const char *file,
           log_e level);

private:
  /**
   * Static private std::ofstream for creating a logfile.
   *
   * The log_file_ is created at the startup and all calls to the log() function
   * are writing into the same file.
   */
  static std::ofstream log_file_;

  /**
   * Check if the log file could be opened.
   */
  static bool file_open_;

  /**
   * Log level being specified during compilation.
   *
   * The log level specifies which log messages are being printed. All higher
   * log levels will be printed then the specified log level.
   */
  static log_e log_level_;
};

#endif /* EventLogger.hpp */
