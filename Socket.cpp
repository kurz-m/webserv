#include "Socket.hpp"

Socket::Socket(addrinfo_t& info) : type_(LISTEN), info_(info) {
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

Socket::Socket(int sockfd) : type_(CONNECT) {
  // accept
}
