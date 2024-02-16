#include "Server.hpp"
#include <iostream>

int main(void) {
  Server serv("3490");

  serv.startup();
  serv.run();

  return 0;
}
