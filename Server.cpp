#include "Server.hpp"
#include <iostream>

Server::Server(const std::string &port)
    : hints_(), port_(port), sockfd_nb_(0), timeout_(2500) {
  hints_.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints_.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints_.ai_flags = AI_PASSIVE;     // Fill in my IP for me
  (void)sockfd_nb_;
}

Server::~Server() { freeaddrinfo(servinfo_); }

void Server::bind_sock(std::map<int, Socket>::iterator &it) {
  if (bind(it->first, it->second.info_.ai_addr, it->second.info_.ai_addrlen) ==
      -1) {
    perror("server: bind");
    close(it->first);
    socket_map_.erase(it);
  } else if (listen(it->first, 10)) {
    perror("server: listen");
    close(it->first);
    socket_map_.erase(it);
  } else {
    it->second.events_ = POLLIN;
    pollfds_.push_back(it->second.to_pollfd());
  }
}

void Server::startup() {
  int status;
  addrinfo_t *p;

  // bind to all interfaces on port
  if ((status = getaddrinfo(NULL, port_.c_str(), &hints_, &servinfo_)) != 0) {
    throw std::exception();
  }

  for (p = servinfo_; p != NULL; p = p->ai_next) {
    try {
      Socket sock(*p);
      socket_map_.insert(std::pair<int, Socket>(sock.sockfd_, sock));
    } catch (std::exception &e) {
    }
  }

  bool ipv6found = false;

  for (std::map<int, Socket>::iterator it = socket_map_.begin();
       it != socket_map_.end(); it++) {
    if (it->second.info_.ai_family == PF_INET6) {
      int no = 0;
      if (setsockopt(it->first, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) ==
          -1) {
        perror("Could not set IPV6_V6ONLY to false");
      } else {
        bind_sock(it);
        ipv6found = true;
        break;
      }
    }
  }

  if (!ipv6found) {
    for (std::map<int, Socket>::iterator it = socket_map_.begin();
         it != socket_map_.end(); it++) {
      bind_sock(it);
      break;
    }
  }

  // could not bind and listen to any socket.
  if (socket_map_.size() < 1) {
    throw std::exception();
  }
}

void Server::run() {
  int num_events;

  while (true) {
    num_events = poll(&pollfds_[0], pollfds_.size(), timeout_);
    if (num_events < 0) {
      perror("server: poll");
      throw std::exception();
    }
    std::vector<pollfd_t>::iterator it;
    if (num_events > 0) {
      for (it = pollfds_.begin(); it != pollfds_.end(); it++) {
        std::cout << "Fd: " << it->fd << " from polling" << std::endl;
        if (!(it->events & it->revents)) {
          continue;
        }
        switch (socket_map_[it->fd].type_) {
        case Socket::LISTEN:
          accept_new_connection(socket_map_[it->fd]);
          break;

        case Socket::CONNECT:
          handle_client(socket_map_[it->fd]);
          break;
        default:
          break;
        }
      }
    }
  }
}

void Server::accept_new_connection(Socket &socket) { (void)socket; }

void Server::handle_client(Socket &socket) { (void)socket; }