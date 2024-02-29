#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "Socket.hpp"
#include <map>
#include <list>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

#define RESET 0

class Server {
public:
  Server(const std::string &port);
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
  addrinfo_t hints_;
  addrinfo_t* servinfo_;
  const std::string port_;
  int timeout_;
  std::string webroot_; // root directory for webserver

friend class Socket;
};

#endif // __SERVER_HPP__
