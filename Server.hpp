#ifndef __SERVER_HPP__
#define __SERVER_HPP__

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
  addrinfo_t hints_;
  addrinfo_t* servinfo_;
  const std::string port_;
  size_t sockfd_nb_;
};

#endif // __SERVER_HPP__
