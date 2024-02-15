#include "Server.hpp"

Server::Server(const std::string& port) : port_(port) {
  memset(&hints_, 0, sizeof(hints_));
  hints_.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints_.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints_.ai_flags = AI_PASSIVE;     // Fill in my IP for me
}

void Server::startup() {
  int status;
  struct addrinfo* p;

  // bind to all interfaces on port 
  if ((status = getaddrinfo(NULL, port_.c_str(), &hints_, &servinfo_)) != 0) {
    throw std::exception();
  }

  for (p = servinfo_; p != NULL; p = p->ai_next) {
    try {
      Socket sock(*p);
      socket_map_[sock.get_sockfd()] = sock;
    } catch (std::exception &e) {}
  }

  for (std::map<int, Socket>::iterator it = socket_map_.begin(); it != socket_map_.end(); it++) {
    if (bind(it->second.get_sockfd(), it->second.get_sockinfo().ai_addr, it->second.get_sockinfo().ai_addrlen) == -1) {
      close(it->second.get_sockfd());
      perror("server: bind");
      // use some kind of counter to check if at least one socket was bound.
    }
  }

}
