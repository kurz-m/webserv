#include "Socket.hpp"
#include <cstdio>
#include <iostream>

Socket::Socket() : status_(READY), type_(UNDEFINED) {
  buffer_.resize(MAX_BUFFER);
}

Socket::Socket(addrinfo_t info) : status_(READY), type_(LISTEN), info_(info) {
  int yes = 1;
  sockfd_ = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
  if (sockfd_ == -1) {
    throw std::exception();
  }
  if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    throw std::exception();
  }
  buffer_.resize(MAX_BUFFER);
}

Socket::Socket(const Socket &other)
    : sockfd_(other.sockfd_), events_(other.events_), status_(other.status_),
      type_(other.type_), info_(other.info_) {
  buffer_.resize(MAX_BUFFER);
}

Socket &Socket::operator=(const Socket &other) {
  if (this != &other) {
    sockfd_ = other.sockfd_;
    events_ = other.events_;
    status_ = other.status_;
    type_ = other.type_;
    info_ = other.info_;
  }
  return *this;
}

void Socket::check_recv_() {
  if (request_.buffer_.find("\r\n\r\n") == std::string::npos) {
    status_ = URECV;
  }
  // TODO: check content length for finished body???
}

void Socket::receive() {
  ssize_t n;
  char buf[Socket::MAX_BUFFER + 1] = {0};
  n = recv(sockfd_, buf, Socket::MAX_BUFFER, MSG_DONTWAIT);
  if (n < 0) {
    std::cerr << "recv failed!" << std::endl;
    throw std::exception();
  }
  request_.buffer_ += std::string(buf);

}

Socket::~Socket() {}

Socket::Socket(int sockfd, const Socket &listen_sock)
    : type_(CONNECT), info_() {
  socklen_t socklen = listen_sock.info_.ai_addrlen;

  sockfd_ = accept(sockfd, listen_sock.info_.ai_addr, &socklen);
  if (sockfd_ == -1) {
    perror("server: accept");
    throw std::exception();
  }
  events_ = POLLIN;
  status_ = READY;
}

pollfd_t Socket::to_pollfd() {
  return (pollfd_t){.fd = sockfd_, .events = events_, .revents = 0};
}
