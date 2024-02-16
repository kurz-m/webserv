#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct addrinfo addrinfo_t;

typedef struct pollfd pollfd_t;

class Server;

class Socket {
public:
  Socket();
  Socket(addrinfo_t info);
  Socket(int sockfd, const Socket& listen_sock);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  ~Socket();

  pollfd_t to_pollfd();

  enum status { READY, URECV, USEND, CLOSED };
  enum type { LISTEN, CONNECT, UNDEFINED };

private:

  std::string buffer_;
  int sockfd_;
  short events_;
  status status_;
  type type_;
  addrinfo_t info_;

  const static size_t MAX_BUFFER = 1024;

  friend class Server;
};

#endif // __SOCKET_HPP__
