#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct pollfd pollfd_t;

class Server;

class Socket {
public:
  Socket(struct addrinfo& info);
  Socket(int sockfd);
  Socket &operator=(const Socket &other);
  ~Socket();

  pollfd_t to_pollfd();
  int get_sockfd();
  short get_events();
  short get_revents();
  void set_events();
  void set_revents();
  struct addrinfo get_sockinfo();

  enum status {READY, URECV, USEND, CLOSED};
  enum type {LISTEN, CONNECT};

private:
  Socket(const Socket &other);

  int sockfd_;
  short events_;
  short revents_;
  status status_;
  type type_;
  struct addrinfo info_;

  friend class Server;

};

#endif // SOCKET_HPP