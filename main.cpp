#include "Server.hpp"
#include <iostream>

int main(void) {
  try {
    Server serv("3490");
    serv.startup();
    serv.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
