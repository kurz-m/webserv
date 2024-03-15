#include "Socket.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

Socket::Socket(pollfd &pollfd, const ServerBlock &config)
    : config_(config), pollfd_(pollfd), status_(ISocket::READY_RECV) {}

Socket::Socket(const Socket &other)
    : config_(other.config_), pollfd_(other.pollfd_) {
  *this = other;
}

Socket &Socket::operator=(const Socket &other) {
  if (this != &other) {
    pollfd_ = other.pollfd_;
    status_ = other.status_;
  }
  return *this;
}

Socket::~Socket() {}

void Socket::check_poll_err_() {
  if (status_ == ISocket::CLOSED) {
    return;
  }
  if (pollfd_.revents & (POLLERR | POLLNVAL | POLLHUP)) {
    std::cout << "client: " << pollfd_.fd << " connection error." << std::endl;
    status_ = ISocket::CLOSED;
  }
  if (!(pollfd_.events & pollfd_.revents)) {
    if (check_timeout_()) {
      status_ = ISocket::CLOSED;
    }
  }
}
