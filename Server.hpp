#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "Socket.hpp"
#include <map>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

class Server {
public:
  Server(const std::string &port);
  ~Server();

  void startup();
  void run();

private:
  void accept_new_connection(Socket& socket);
  void handle_client(Socket& socket);


  std::vector<pollfd_t> pollfds_;
  std::map<int, Socket> socket_map_;
  addrinfo_t hints_;
  addrinfo_t* servinfo_;
  const std::string port_;
  size_t sockfd_nb_; // for later use
  int timeout_;

};

#endif // __SERVER_HPP__
