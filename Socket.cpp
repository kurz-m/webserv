#include "Socket.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

Socket::Socket(pollfd &pollfd, const ServerBlock &config)
    : config_(config), pollfd_(pollfd),
      status_(READY) {}

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
