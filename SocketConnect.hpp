#ifndef __SOCKET_CONNECT_HPP
#define __SOCKET_CONNECT_HPP

#include <list>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Socket.hpp"

class SocketConnect : public Socket {
public:
  class SendRecvError : public std::exception {
  public:
    const char *what() const throw();
  };

  SocketConnect(pollfd_t &pollfd, const ServerBlock &config,
                int timeout = DEFAULT_TIMEOUT);
  SocketConnect(const SocketConnect &cpy);
  SocketConnect &operator=(const SocketConnect &other);
  ~SocketConnect();

  ISocket::status handle(std::map<int, ISocket> &client_map,
              std::list<pollfd_t> &poll_list);

  SocketConnect *clone() const;

private:
  void receive_();
  void respond_();
  void send_response_();
  void check_recv_();
  bool check_timeout_() const;

  HTTPRequest request_;
  HTTPResponse response_;
  int timeout_;
  std::time_t timestamp_;

  friend class Server;
};

#endif /* SocketConnect.hpp */
