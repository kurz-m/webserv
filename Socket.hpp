#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <ctime>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ISocket.hpp"

class Server;
class ISocket;

class Socket {
public:
  Socket(pollfd_t &pollfd, const ServerBlock &config);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  virtual ~Socket() = 0;

  virtual bool check_timeout_() const = 0;
  virtual Socket *clone() const = 0;
  virtual ISocket::status
  handle(std::map<int, ISocket> &sock_map,
         std::list<pollfd_t> &poll_list) = 0;

protected:
  const ServerBlock &config_;
  pollfd_t &pollfd_;
  ISocket::status status_;

  void check_poll_err_();

  friend class Server;
  friend class HTTPRequest;
  friend class ISocket;
};

#endif // __SOCKET_HPP__
