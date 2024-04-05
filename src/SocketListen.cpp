#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>

#include "EventLogger.hpp"
#include "SocketListen.hpp"
#include "Token.hpp"

SocketListen::SocketListen(pollfd_t &pollfd, const ServerBlock &config)
    : Socket(pollfd, config) {}

SocketListen::SocketListen(const SocketListen &cpy)
    : Socket(cpy) {}

SocketListen &SocketListen::operator=(const SocketListen &other) {
  if (this != &other) {
    Socket::operator=(other);
  }
  return *this;
}

SocketListen::~SocketListen() {}

ISocket::status SocketListen::handle(std::map<int, ISocket> &sock_map,
                                     std::list<pollfd_t> &poll_list) {
  if (pollfd_.revents & POLLIN) {
    try {
      LOG_INFO("I am trying to establish a new connecion");
      int sockfd = accept(pollfd_.fd, NULL, NULL);
      if (sockfd < 0) {
        LOG_WARNING("accept: " + std::string(strerror(errno)));
        return ISocket::READY_RECV;
      }

      pollfd_t pollfd =
          (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
      poll_list.push_back(pollfd);
      sock_map.insert(
          std::make_pair(sockfd, ISocket(poll_list.back(), config_, DEFAULT_TIMEOUT)));
      std::ostringstream oss;
      oss << "accepted client fd: " << sockfd;
      LOG_INFO(oss.str());
      fcntl(sockfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
    } catch (const std::exception &e) {
      LOG_ERROR(std::string("Accept failed: ") + e.what());
    }
  }
  return ISocket::READY_RECV;
}

bool SocketListen::check_timeout_() const { return false; }

SocketListen *SocketListen::clone() const { return new SocketListen(*this); }
