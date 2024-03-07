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

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Settings.hpp"

#define DEFAULT_TIMEOUT 15
#define RESET 0

typedef struct addrinfo addrinfo_t;

typedef struct pollfd pollfd_t;

class Server;

class Socket {
public:
  enum status { READY, URECV, USEND, CLOSED };

  Socket(pollfd_t &pollfd, const ServerBlock &config);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  virtual ~Socket() = 0;

  // virtual bool handle() = 0;
  virtual bool check_timeout() const = 0;

protected:
  const ServerBlock &config_;
  pollfd_t &pollfd_;
  status status_;

  friend class Server;
  friend class HTTPRequest;
};

#endif // __SOCKET_HPP__
