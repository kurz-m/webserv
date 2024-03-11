#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <stdexcept>

#include "Server.hpp"
#include "SocketListen.hpp"
#include "Token.hpp"

extern sig_atomic_t g_signal;

Server::Server(const HttpBlock &config)
    : config_(config), poll_timeout_(2500) {}

Server::~Server() {}

void Server::startup() {
#ifdef __verbose__
  std::cout << "starting the server in __verbose__ mode" << std::endl;
#endif
  std::vector<ServerBlock>::const_iterator it;
  for (it = config_.servers.begin(); it != config_.servers.end(); ++it) {
    create_listen_socket_(*it);
  }
}

void Server::create_listen_socket_(const ServerBlock &config) {
  int status;
  addrinfo_t hints = {}; // make sure for the hints to be empty!
  addrinfo_t *servinfo;
  addrinfo_t *p;

  hints.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints.ai_flags = AI_PASSIVE;     // Fill in my IP for me

  // bind to all interfaces on port
  if ((status = getaddrinfo(NULL, config.find(Token::LISTEN).str_val.c_str(),
                            &hints, &servinfo)) != 0) {
    throw std::runtime_error(std::string("server: getaddrinfo: ") + std::strerror(errno));
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      std::cerr << "server: socket: " << std::strerror(errno) << std::endl;
      continue;
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "server: setsockopt: " << std::strerror(errno) << std::endl;
      continue;
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      std::cerr << "server: bind: " << std::strerror(errno) << std::endl;
      close(sockfd);
      continue;
    }
    if (listen(sockfd, 10)) {
      perror("server: listen");
      std::cerr << "server: listen: " << std::strerror(errno) << std::endl;
      close(sockfd);
      continue;
    }
    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list_.push_back(pollfd);
    client_map_.insert(
        std::make_pair(sockfd, SocketInterface(poll_list_.back(), config, *p)));
    break;
  }

  freeaddrinfo(servinfo);

  if (client_map_.size() < 1) {
    throw std::runtime_error("could not bind and listen to any port!");
  }
}

int Server::do_poll_() {
  std::vector<pollfd_t> pollfds;
  std::list<pollfd_t>::iterator it;
  for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
    pollfds.push_back(*it);
  }
  int num_events = poll(&pollfds[0], pollfds.size(), poll_timeout_);
  if (num_events < 0) {
    perror("server: poll");
    throw std::runtime_error(std::string(std::strerror(errno)));
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
  for (it = poll_list_.begin(); it != poll_list_.end();) {
    std::cout << "Fd: " << it->fd << " from polling" << std::endl;
    if ((it->revents & POLLERR) | (it->revents & POLLNVAL)) {
      std::cout << "client: " << it->fd << " connection error." << std::endl;
      close(it->fd);
      client_map_.erase(it->fd);
      it = poll_list_.erase(it);
      continue;
    }
    if (it->revents & POLLHUP) {
      std::cout << "client: " << it->fd << " POLLHUP." << std::endl;
      close(it->fd);
      client_map_.erase(it->fd);
      it = poll_list_.erase(it);
      continue;
    }
    if (!(it->events & it->revents)) {
      if (client_map_.at(it->fd).check_timeout()) {
        std::cout << "client: " << it->fd << " Timeout." << std::endl;
        close(it->fd);
        client_map_.erase(it->fd);
        it = poll_list_.erase(it);
      } else {
        ++it;
      }
      continue;
    }
    client_map_.at(it->fd).handle(client_map_, poll_list_);
    ++it;
  }
}

void Server::run() {
  while (g_signal == 0) {
    do_poll_();
#ifdef __verbose__
    std::cout << "polled" << std::endl;
#endif
    // if (num_events > 0) {
    event_handler_();
    // }
  }
}
