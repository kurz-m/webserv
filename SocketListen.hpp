#ifndef __SOCKET_LISTEN_HPP
#define __SOCKET_LISTEN_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "ISocket.hpp"

class SocketListen : public Socket {
public:
  SocketListen(pollfd &pollfd, const ServerBlock &config,
               const addrinfo_t &info);
  SocketListen(const SocketListen &cpy);
  SocketListen &operator=(const SocketListen &other);
  ~SocketListen();

  ISocket::status handle(std::map<int, ISocket> &client_map,
              std::list<pollfd_t> &poll_list);
  bool check_timeout_() const;
  SocketListen *clone() const;

private:
  addrinfo_t servinfo_;

  friend class Server;
};

#endif /* SocketListen.hpp */
