#include "ISocket.hpp"
#include "SocketConnect.hpp"
#include "SocketListen.hpp"

ISocket::ISocket(pollfd_t &pollfd, const ServerBlock &config,
                 const addrinfo_t &info) {
  sock_ = new SocketListen(pollfd, config, info);
}
ISocket::ISocket(pollfd_t &pollfd, const ServerBlock &config,
                 int timeout /* = DEFAULT_TIMEOUT */) {
  sock_ = new SocketConnect(pollfd, config, timeout);
}

ISocket::ISocket(const ISocket &obj)
    : sock_(obj.sock_ ? obj.sock_->clone() : NULL) {}

ISocket &ISocket::operator=(const ISocket &obj) {
  if (this != &obj) {
    if (sock_ != NULL) {
      delete sock_;
    }
    sock_ = obj.sock_->clone();
  }
  return *this;
}
ISocket::~ISocket() { delete sock_; }

ISocket::status ISocket::handle(std::map<int, ISocket> &sock_map, std::list<pollfd_t>& poll_list) {
  return sock_->handle(sock_map, poll_list);
}

bool ISocket::check_timeout_() const { return sock_->check_timeout_(); }
