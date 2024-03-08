#include "Lexer.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Settings.hpp"
#include <fstream>
#include <iostream>
#include <vector>

void print_usage() {
  std::cout << "Usage: \n\t./webserv [path/to/config]" << std::endl;
}

int main(int argc, char **argv) {
  std::string buffer;
  const char *config_path = "./config/default.conf";
  if (argc > 2) {
    print_usage();
    return 1;
  }
  if (argc == 2) {
    config_path = argv[1];
  }
  std::ifstream file(config_path);
  if (!file.is_open()) {
    std::cerr << "could not open config file!" << std::endl;
    return 1;
  }
  try {
    buffer.assign(std::istreambuf_iterator<char>(file),
                  std::istreambuf_iterator<char>());
    Lexer lexer(buffer);
    Parser parser(lexer);
    HttpBlock settings = parser.parse_config();
    Server server(settings);
    server.startup();
    server.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
