#include "SocketInterface.hpp"
#include "SocketConnect.hpp"
#include "SocketListen.hpp"

// SocketInterface::SocketInterface() : sock_(NULL) {}

SocketInterface::SocketInterface(pollfd &pollfd, const ServerBlock &config,
                                 const addrinfo_t &info) {
  sock_ = new SocketListen(pollfd, config, info);
}
SocketInterface::SocketInterface(pollfd_t &pollfd, const ServerBlock &config,
                                 int timeout /* = DEFAULT_TIMEOUT */) {
  sock_ = new SocketConnect(pollfd, config, timeout);
}

SocketInterface::SocketInterface(const SocketInterface &obj) : sock_(NULL) { *this = obj; }

SocketInterface &SocketInterface::operator=(const SocketInterface &obj) {
  if (this != &obj) {
    if (sock_ != NULL) {
      delete sock_;
    }
    sock_ = obj.sock_->clone();
  }
  return *this;
}
SocketInterface::~SocketInterface() {
  delete sock_;
}

void SocketInterface::handle(std::map<int, SocketInterface> &sock_map, std::list<pollfd_t>& poll_list) {
  sock_->handle(sock_map, poll_list);
}

bool SocketInterface::check_timeout() const
{
  return sock_->check_timeout();
}
