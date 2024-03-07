#include "SocketListen.hpp"
#include "Token.hpp"
#include <iostream>

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
}

SocketConnect
SocketListen::new_connection(std::list<pollfd_t> &poll_list,
                                    std::map<int, Socket> &client_map) {
  try {
#ifdef __verbose__
    std::cout << "I am trying to establish a new connecion" << std::endl;
#endif
    int sockfd = accept(pollfd_.fd, servinfo_.ai_addr, &servinfo_.ai_addrlen);
    if (sockfd < 0) {
      perror("accept");
      return;
    }

    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list.push_back(pollfd);
    SocketConnect new_sock(poll_list.back(), config_);
    client_map.insert(std::make_pair(sockfd, new_sock));
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

// void SocketListen::handle(std::list<pollfd_t> &poll_list,
//                           std::map<int, Socket> &client_map) {
//   new_connection_(poll_list, client_map);
// }

bool SocketListen::check_timeout() const { return true; }
