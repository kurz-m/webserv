#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

typedef struct addrinfo addrinfo_t;

typedef struct pollfd pollfd_t;

class Server;

class Socket {
public:
  enum status { READY, URECV, USEND, CLOSED };
  enum type { LISTEN, CONNECT, UNDEFINED };

  // Socket();
  Socket(pollfd& pollfd, type type);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  ~Socket();

  // pollfd_t to_pollfd();
  void receive();
  void send_response();

private:
  HTTPRequest request_;
  HTTPResponse response_;
  pollfd_t& pollfd_;
  status status_;
  type type_;

  void check_recv_();

  friend class Server;
};

#endif // __SOCKET_HPP__
