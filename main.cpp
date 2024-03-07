#include "Server.hpp"
#include <iostream>
#include <vector>
#include "Settings.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"

void print_usage() {
  std::cout << "Usage: \n\t./webserv [path/to/config]" << std::endl;
}

int main(int argc, char **argv) {
  std::string config_path = "./config";
  if (argc > 2) {
    print_usage();
    return 1;
  }
  if (argc == 2) {
    config_path = std::string(argv[1]);
  }
  try {
    Lexer lexer(config_path);
    Parser parser(lexer);
    HttpBlock settings = parser.parse_config();
    Server server(settings);
    server.startup();
    server.run();
    // std::vector<Server> serv_vec;
    // std::vector<ServerBlock>::iterator it;
    // for (it = settings.servers.begin(); it != settings.servers.end(); ++it) {
    //   Server serv = Server(*it);
    //   serv.startup();
    //   serv.run();
    //   serv_vec.push_back(serv);
    // }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
