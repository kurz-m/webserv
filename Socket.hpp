#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <ctime>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <list>

#include "SocketInterface.hpp"

#define RESET 0


class Server;
class SocketInterface;

class Socket {
public:
  enum status { READY, URECV, USEND, CLOSED };

  Socket(pollfd_t &pollfd, const ServerBlock &config);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  virtual ~Socket() = 0;

  // virtual bool handle() = 0;
  virtual bool check_timeout() const = 0;
  virtual Socket *clone() const = 0;
  virtual void handle(std::map<int, SocketInterface> &sock_map,
                      std::list<pollfd_t> &poll_list) = 0;

protected:
  const ServerBlock &config_;
  pollfd_t &pollfd_;
  status status_;

  friend class Server;
  friend class HTTPRequest;
  friend class SocketInterface;
};

#endif // __SOCKET_HPP__
