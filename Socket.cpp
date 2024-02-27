#include "Socket.hpp"
#include <cstdio>
#include <iostream>

// Socket::Socket() : pollfd_((pollfd_t){}), status_(READY), type_(UNDEFINED) {}

Socket::Socket(pollfd& pollfd, type type) : pollfd_(pollfd), status_(READY), type_(type) {
  int yes = 1;
  if (setsockopt(pollfd_.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    throw std::exception();
  }
}

Socket::Socket(const Socket &other)
    : request_(other.request_), response_(other.response_), pollfd_(other.pollfd_), status_(other.status_),
      type_(other.type_) {}

Socket &Socket::operator=(const Socket &other) {
  if (this != &other) {
    request_ = other.request_;
    response_ = other.response_;
    pollfd_ = other.pollfd_;
    status_ = other.status_;
    type_ = other.type_;
  }
  return *this;
}

void Socket::check_recv_() {
  if (request_.buffer_.find("\r\n\r\n") == std::string::npos) {
    status_ = URECV;
  }
  // TODO: check content length for finished body???
  else status_ = READY;
  // Set events_ according to status_
  if (status_ == READY) {
    pollfd_.events = POLLOUT;
  }
}

void Socket::receive() {
  ssize_t n;
  char buf[HTTPBase::MAX_BUFFER + 1] = {0};
  n = recv(pollfd_.fd, buf, HTTPBase::MAX_BUFFER, MSG_DONTWAIT);
  if (n < 0) {
    std::cerr << "recv failed!" << std::endl;
    throw std::exception();
  }
  request_.buffer_ += std::string(buf);
  check_recv_();
}

Socket::~Socket() {}

// Socket::Socket(int sockfd, const Socket &listen_sock)
//     : type_(CONNECT), info_() {
//   socklen_t socklen = listen_sock.info_.ai_addrlen;

//   sockfd_ = accept(sockfd, listen_sock.info_.ai_addr, &socklen);
//   if (sockfd_ == -1) {
//     perror("server: accept");
//     throw std::exception();
//   }
//   events_ = POLLIN;
//   status_ = READY;
// }

// pollfd_t Socket::to_pollfd() {
//   return (pollfd_t){.fd = sockfd_, .events = events_, .revents = 0};
// }
