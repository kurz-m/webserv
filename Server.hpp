#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "Socket.hpp"
#include <map>
#include <list>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>
#include "Settings.hpp"
#include "Parser.hpp"

#define RESET 0

class Server {
public:
  Server(const ServerBlock &config);
  ~Server();

  void startup();
  void run();

private:
  void accept_new_connection(Socket& socket);
  void handle_client(Socket& socket);
  int  do_poll_();
  void event_handler_();

  std::list<pollfd_t> poll_list_;
  std::map<int, Socket> client_map_;
  // std::map<std::string, std::string> config_map_;
  addrinfo_t hints_;
  addrinfo_t* servinfo_;
  int timeout_;
  std::string port_;
  std::string webroot_; // root directory for webserver
  const ServerBlock& config_;  // all settings from config

friend class Socket;
};

#endif // __SERVER_HPP__
