#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct addrinfo addrinfo_t;

class Socket {
public:
  Socket(addrinfo_t& info);
  Socket(int sockfd);
  Socket &operator=(const Socket &other);
  ~Socket();

  struct pollevent to_pollfd();
  int get_sockfd();
  short get_events();
  short get_revents();
  void set_events();
  void set_revents();
  addrinfo_t get_sockinfo();

  enum status {READY, URECV, USEND, CLOSED};
  enum type {LISTEN, CONNECT};

private:
  Socket(const Socket &other);

  int sockfd_;
  short events_;
  short revents_;
  status status_;
  type type_;
  addrinfo_t info_;

};

#endif // __SOCKET_HPP__
