#ifndef __SOCKET_INTERFACE_HPP__
#define __SOCKET_INTERFACE_HPP__

#include <list>

#include "Settings.hpp"

typedef struct addrinfo addrinfo_t;
typedef struct pollfd pollfd_t;

#define DEFAULT_TIMEOUT 15

class Socket;

class SocketInterface {
public:
  // SocketInterface();
  SocketInterface(pollfd &pollfd, const ServerBlock &config,
                  const addrinfo_t &info);
  SocketInterface(pollfd_t &pollfd, const ServerBlock &config,
                  int timeout = DEFAULT_TIMEOUT);

  SocketInterface(const SocketInterface &obj);
  SocketInterface &operator=(const SocketInterface &obj);
  ~SocketInterface();

  Socket::status handle(std::map<int, SocketInterface> &sock_map,
              std::list<pollfd_t> &poll_list);
  bool check_timeout() const;

private:
  Socket *sock_;

friend class Server;
};

#endif
