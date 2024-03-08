#ifndef __SOCKET_LISTEN_HPP__
#define __SOCKET_LISTEN_HPP__

#include "Server.hpp"
#include "Socket.hpp"
#include "SocketConnect.hpp"

class SocketListen : public Socket {
public:
  SocketListen(pollfd &pollfd, const ServerBlock &config,
               const addrinfo_t &info);
  SocketListen(const SocketListen &cpy);
  SocketListen &operator=(const SocketListen &other);
  ~SocketListen();

  SocketConnect new_connection(std::list<pollfd_t> &poll_list,
                                      std::map<int, Socket> &client_map);
  bool check_timeout() const;

private:
  addrinfo_t servinfo_;

  friend class Server;
};

#endif // __SOCKET_LISTEN_HPP__