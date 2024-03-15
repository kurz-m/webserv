#include "Socket.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

Socket::Socket(pollfd &pollfd, const ServerBlock &config)
    : config_(config), pollfd_(pollfd), status_(READY_RECV) {}

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

Socket::status Socket::check_poll_err_() const
{
  if (pollfd_.revents & (POLLERR | POLLNVAL | POLLHUP)) {
      std::cout << "client: " << pollfd_.fd << " connection error." << std::endl;

    }
  if (!(pollfd_.events & pollfd_.revents)) {
    if (client_map_.at(pollfd_.fd).check_timeout()) {
      std::cout << "client: " << pollfd_.fd << " Timeout." << std::endl;
      close(pollfd_.fd);
      client_map_.erase(pollfd_.fd);
      it = poll_list_.erase(it);
    } else {
      ++it;
    }
  }
}
