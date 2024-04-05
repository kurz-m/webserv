#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "EventLogger.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Settings.hpp"

sig_atomic_t g_signal = 0;

void print_usage() {
  std::cout << "Usage: \n\t./webserv [path/to/config]" << std::endl;
}

static void signal_handler(int signo) { g_signal = signo; }

static void setup_signal_handler() {
  struct sigaction act;

  sigemptyset(&act.sa_mask);
  act.sa_handler = signal_handler;
  sigaction(SIGINT, &act, NULL);
}

int main(int argc, char **argv) {
  setup_signal_handler();
  std::string buffer;
  EventLogger logger("logs/webserv.log");
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
    LOG_ERROR("could not open config file!");
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
    if (g_signal == 0) {
      std::cerr << e.what() << std::endl;
      return 1;
    }
  }
  std::cout << "Goodbye!" << std::endl;
  return 0;
}
