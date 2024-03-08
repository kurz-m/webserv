#ifndef __SOCKET_CONNECT_HPP__
#define __SOCKET_CONNECT_HPP__

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

  void handle();
  bool check_timeout() const;

private:
  void receive_();
  void send_response_();
  void check_recv_();
  void resolve_uri_();
  void interpret_request_headers_();

  HTTPRequest request_;
  HTTPResponse response_;
  int timeout_;
  std::time_t timestamp_;

  friend class Server;
};

#endif // __SOCKET_CONNECT_HPP__