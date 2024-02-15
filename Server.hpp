#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <sys/poll.h>
#include <unistd.h>
#include "Socket.hpp"

typedef struct pollfd pollfd_t;

class Server {
public:
  Server(const std::string& port);
  ~Server();

  void startup();

private:
  std::vector<pollfd_t> sockets_;
  std::map<int, Socket> socket_map_;
  struct addrinfo hints_;
  struct addrinfo* servinfo_;
  const std::string port_;
};

#endif // SERVER_HPP