#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ctime>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

#define DEFAULT_TIMEOUT 15

typedef struct addrinfo addrinfo_t;

typedef struct pollfd pollfd_t;

class Server;

class Socket {
public:
  enum status { READY, URECV, USEND, CLOSED };
  enum type { LISTEN, CONNECT, UNDEFINED };

  // Socket();
  Socket(pollfd& pollfd, type type, int timeout = DEFAULT_TIMEOUT);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  ~Socket();

  void receive();
  void send_response();

  class SendRecvError : public std::exception {
  public:
    const char *what() const throw();
  };

private:
  HTTPRequest request_;
  HTTPResponse response_;
  pollfd_t& pollfd_;
  status status_;
  type type_;
  int timeout_;
  std::time_t timestamp_;

  void check_recv_();

  friend class Server;
  friend class HTTPRequest;
};

#endif // __SOCKET_HPP__
