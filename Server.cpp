#include "Server.hpp"
#include <cstdio>
#include <iostream>

Server::Server(const std::string &port)
    : hints_(), port_(port), sockfd_nb_(0), timeout_(2500) {
  hints_.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints_.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints_.ai_flags = AI_PASSIVE;     // Fill in my IP for me
  (void)sockfd_nb_;
}

Server::~Server() { freeaddrinfo(servinfo_); }

void Server::startup() {
  int status;
  addrinfo_t *p;

  // bind to all interfaces on port
  if ((status = getaddrinfo(NULL, port_.c_str(), &hints_, &servinfo_)) != 0) {
    throw std::exception();
  }

  for (p = servinfo_; p != NULL; p = p->ai_next) {
    int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      continue;
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("server: bind");
      close(sockfd);
      continue;
    }
    if (listen(sockfd, 10)) {
      perror("server: listen");
      close(sockfd);
    }
    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list_.push_back(pollfd);
    Socket sock(poll_list_.back(), Socket::LISTEN);
    client_map_.insert(std::pair<int, Socket>(sockfd, sock));
    break;
  }

  // could not bind and listen to any socket.
  if (client_map_.size() < 1) {
    throw std::exception();
  }
}

int Server::do_poll_() {
  std::vector<pollfd_t> pollfds;
  std::list<pollfd_t>::iterator it;
  for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
    pollfds.push_back(*it);
  }
  int num_events = poll(&pollfds[0], pollfds.size(), timeout_);
  if (num_events < 0) {
    perror("server: poll");
    throw std::exception();
  }
  if (num_events > 0) {
    int i = 0;
    std::list<pollfd_t>::iterator it;
    for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
      it->revents = pollfds[i++].revents;
    }
  }
  return num_events;
}

void Server::event_handler_() {
  std::list<pollfd_t>::iterator it;
  for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
    std::cout << "Fd: " << it->fd << " from polling" << std::endl;
    if (!(it->events & it->revents)) {
      continue;
    }
    switch (client_map_.at(it->fd).type_) {
    case Socket::LISTEN:
      std::cout << "In switch LISTEN" << std::endl;
      accept_new_connection(client_map_.at(it->fd));
      break;

    case Socket::CONNECT:
      std::cout << "In switch CONNECT" << std::endl;
      handle_client(client_map_.at(it->fd));
      break;
    default:
      break;
    }
  }
}

void Server::run() {
  while (true) {
    int num_events = do_poll_();
    std::cout << "polled" << std::endl;
    if (num_events > 0) {
      event_handler_();
    }
  }
}

void Server::accept_new_connection(Socket &socket) {
  try {
    std::cout << "I am trying to establish a new connecion" << std::endl;
    int sockfd =
        accept(socket.pollfd_.fd, servinfo_->ai_addr, &servinfo_->ai_addrlen);
    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list_.push_back(pollfd);
    Socket new_sock(poll_list_.back(), Socket::CONNECT);
    client_map_.insert(std::make_pair(sockfd, new_sock));
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

void Server::handle_client(Socket &socket) {
  std::cout << "handle client: " << socket.pollfd_.fd << std::endl;
  switch (socket.pollfd_.revents) {
  case POLLIN:
    socket.receive();
    if (socket.status_ == Socket::READY) {
      std::cout << socket.request_.buffer_ << std::endl;
    }
    break;
  case POLLOUT:
    std::string buffer = "HTTP/1.1 200 OK\r\nContent-Length: "
                         "26\r\n\r\n<html>Hello, World!</html>"; //
    send(socket.pollfd_.fd, buffer.c_str(), buffer.size(), 0);
    socket.pollfd_.events = POLLIN;
    break;
  }
}
