#ifndef __SOCKET_INTERFACE_HPP__
#define __SOCKET_INTERFACE_HPP__

#include <list>

#include "Settings.hpp"

typedef struct addrinfo addrinfo_t;
typedef struct pollfd pollfd_t;

#define DEFAULT_TIMEOUT 15

class Socket;

class ISocket {
public:
  // need this here because we need it here.
  enum status {
    PREPARE_SEND,
    READY_SEND,
    READY_RECV,
    URECV,
    USEND,
    WAITCGI,
    CLOSED
  };

  ISocket(pollfd &pollfd, const ServerBlock &config,
                  const addrinfo_t &info);
  ISocket(pollfd_t &pollfd, const ServerBlock &config,
                  int timeout = DEFAULT_TIMEOUT);

  ISocket(const ISocket &obj);
  ISocket &operator=(const ISocket &obj);
  ~ISocket();

  status handle(std::map<int, ISocket> &sock_map,
              std::list<pollfd_t> &poll_list);
  bool check_timeout_() const;

private:
  Socket *sock_;

friend class Server;
};

#endif
