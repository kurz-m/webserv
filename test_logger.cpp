#include "EventLogger.hpp"

#define LOG_FILE "test-logging.log"

void function2() {
  EventLogger logger;
  logger.log("This is the second one", __LINE__, __FILE__, TEST);
}

void function1() {
  EventLogger logger;
  logger.log("This is the first one", __LINE__, __FILE__, INFO);
}

int main() {
  EventLogger test(LOG_FILE);
  function1();
  function2();
  return 0;
}
