#ifndef SERVER_HPP
#define SERVER_HPP

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
  struct addrinfo hints_;
  struct addrinfo *servinfo_;
  const std::string port_;
  int timeout_;

};

#endif // SERVER_HPP