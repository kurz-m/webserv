#include "Server.hpp"
#include <iostream>
#include <cstdio>

Server::Server(const std::string &port)
    : hints_(), port_(port), sockfd_nb_(0), timeout_(2500) {
  hints_.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints_.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints_.ai_flags = AI_PASSIVE;     // Fill in my IP for me
  (void)sockfd_nb_;
}

Server::~Server() { freeaddrinfo(servinfo_); }

bool Server::bind_sock(std::map<int, Socket>::iterator &it) {
  if (bind(it->first, it->second.info_.ai_addr, it->second.info_.ai_addrlen) ==
      -1) {
    perror("server: bind");
    close(it->first);
    client_map_.erase(it);
    return false;
  }
  if (listen(it->first, 10)) {
    perror("server: listen");
    close(it->first);
    client_map_.erase(it);
    return false;
  }
  it->second.events_ = POLLIN;
  pollfds_.push_back(it->second.to_pollfd());
  return true;
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
      client_map_.insert(std::pair<int, Socket>(sock.sockfd_, sock));
    } catch (std::exception &e) {
    }
  }

  bool ipv6found = false;

  for (std::map<int, Socket>::iterator it = client_map_.begin();
      it != client_map_.end(); it++) {
    if (!ipv6found && it->second.info_.ai_family == PF_INET6) {
      int no = 0;
      if (setsockopt(it->first, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) ==
          -1) {
        perror("Could not set IPV6_V6ONLY to false");
        continue;
      }
      if (bind_sock(it)) {
        ipv6found = true;
        Socket sock = it->second;
        client_map_.clear();
        client_map_[sock.sockfd_] = sock;
        break;
      }
    }
  }

  if (!ipv6found) {
    for (std::map<int, Socket>::iterator it = client_map_.begin();
        it != client_map_.end(); it++) {
      if (bind_sock(it)) {
        Socket sock = it->second;
        client_map_.clear();
        client_map_[sock.sockfd_] = sock;
        break;
      }
    }
  }

  // could not bind and listen to any socket.
  if (client_map_.size() < 1) {
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
    std::cout << "polled" << std::endl;
    if (num_events > 0) {
      for (size_t i = 0; i < pollfds_.size(); i++) {
        std::cout << "Fd: " << pollfds_[i].fd << " from polling" << std::endl;
        sleep(1);
        if (!(pollfds_[i].events & pollfds_[i].revents)) {
          continue;
        }
        switch (client_map_[pollfds_[i].fd].type_) {
        case Socket::LISTEN:
          std::cout << "In switch LISTEN" << std::endl;
          accept_new_connection(client_map_[pollfds_[i].fd]);
          break;

        case Socket::CONNECT:
          std::cout << "In switch CONNECT" << std::endl;
          handle_client(client_map_[pollfds_[i].fd], pollfds_[i]);
          break;
        default:
          break;
        }
      }
    }
  }
}

void Server::accept_new_connection(Socket &socket) {
  try {
    std::cout << "I am trying to establish a new connecion" << std::endl;
    Socket new_sock(socket.sockfd_, socket);
    client_map_[new_sock.sockfd_] = new_sock;
    pollfds_.push_back(new_sock.to_pollfd());
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

void Server::handle_client(Socket &socket, pollfd_t& pollfd) {
  std::cout << "handle client: " << socket.sockfd_ << std::endl;
  ssize_t n;
  char buf[Socket::MAX_BUFFER + 1] = {0};
  switch (pollfd.revents) {
  case POLLIN:
    n = recv(socket.sockfd_, &socket.buffer_[0], Socket::MAX_BUFFER - 1,
            MSG_DONTWAIT);
		(void)n;
    socket.buffer_ += std::string(buf);
    std::cout << socket.buffer_ << std::endl;
    pollfd.events = POLLOUT;
    break;
  case POLLOUT:
    std::string buffer = "HTTP/1.1 200 OK\r\nContent-Length: 26\r\n\r\n<html>Hello, World!</html>"; // 
    send(socket.sockfd_, buffer.c_str(), buffer.size(), 0);
    pollfd.events = POLLIN;
    break;
  }
}
