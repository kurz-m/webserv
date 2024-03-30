#include "SocketListen.hpp"
#include "EventLogger.hpp"
#include "Token.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fcntl.h>

SocketListen::SocketListen(pollfd &pollfd, const ServerBlock &config,
                           const addrinfo_t &info)
    : Socket(pollfd, config), servinfo_(info) {}

SocketListen::SocketListen(const SocketListen &cpy)
    : Socket(cpy), servinfo_(cpy.servinfo_) {
  *this = cpy;
}

SocketListen &SocketListen::operator=(const SocketListen &other) {
  if (this != &other) {
    Socket::operator=(other);
    servinfo_ = other.servinfo_;
  }
  return *this;
}

SocketListen::~SocketListen() {}

ISocket::status SocketListen::handle(std::map<int, ISocket> &client_map,
                                     std::list<pollfd_t> &poll_list) {
  if (pollfd_.revents & POLLIN) {
    try {
      LOG_INFO("I am trying to establish a new connecion");
      int sockfd = accept(pollfd_.fd, servinfo_.ai_addr, &servinfo_.ai_addrlen);
      if (sockfd < 0) {
        LOG_WARNING("accept: " + std::string(strerror(errno)));
        return ISocket::READY_RECV;
      }

      pollfd_t pollfd =
          (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
      poll_list.push_back(pollfd);
      client_map.insert(
          std::make_pair(sockfd, ISocket(poll_list.back(), config_)));
      std::ostringstream oss;
      oss << "accepted client fd: " << sockfd;
      LOG_INFO(oss.str());
      fcntl(sockfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
    } catch (const std::exception &e) {
      std::cerr << "accept failed: " << e.what() << '\n';
    }
  }
  return ISocket::READY_RECV;
}

bool SocketListen::check_timeout_() const { return false; }

SocketListen *SocketListen::clone() const { return new SocketListen(*this); }
