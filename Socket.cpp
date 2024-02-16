#include "Socket.hpp"

Socket::Socket() : type_(UNDEFINED) {}

Socket::Socket(addrinfo_t info) : type_(LISTEN), info_(info) {
  int yes = 1;
  sockfd_ = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
  if (sockfd_ == -1) {
    throw std::exception();
  }
  if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    throw std::exception();
  }
}

Socket::Socket(const Socket &other)
    : sockfd_(other.sockfd_), events_(other.events_), status_(other.status_),
      type_(other.type_), info_(other.info_) {}

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

Socket::~Socket() {}

Socket::Socket(int sockfd) : type_(CONNECT) { (void)sockfd; }

pollfd_t Socket::to_pollfd() {
  return (pollfd_t){.fd = sockfd_, .events = events_, .revents = 0};
}