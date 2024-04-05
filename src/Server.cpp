#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "EventLogger.hpp"
#include "Server.hpp"
#include "SocketListen.hpp"
#include "Token.hpp"

extern sig_atomic_t g_signal;

Server::Server(HttpBlock &config) : config_(config) {}

Server::~Server() {}

void Server::startup() {
  std::vector<ServerBlock>::iterator it;
  for (it = config_.servers.begin(); it != config_.servers.end();) {
    try {
      create_listen_socket_(*it);
      ++it;
    } catch (const std::exception &) {
      LOG_WARNING("server got erased: " + it->server_name);
      it = config_.servers.erase(it);
    }
  }
  if (config_.servers.empty()) {
    throw std::runtime_error("could not create a single server");
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
  if ((status = getaddrinfo(NULL, config.listen.c_str(), &hints, &servinfo)) !=
      0) {
    LOG_ERROR(std::string("server: getaddrinfo: ") + std::strerror(errno));
    throw std::exception();
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      LOG_ERROR(std::string("socket() failed. ") + std::strerror(errno));
      continue;
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      LOG_ERROR(std::string("Setsockopt() failed: ") + std::strerror(errno));
      continue;
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      std::ostringstream oss;
      oss << sockfd;
      LOG_ERROR("Sock FD: " + oss.str() +
                " failed to bind. Error: " + std::strerror(errno));
      close(sockfd);
      continue;
    }
    if (listen(sockfd, 10)) {
      std::ostringstream oss;
      oss << sockfd;
      LOG_ERROR("Sock FD: " + oss.str() +
                " call to listen failed. Error: " + std::strerror(errno));
      close(sockfd);
      continue;
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list_.push_back(pollfd);
    sock_map_.insert(
        std::make_pair(sockfd, ISocket(poll_list_.back(), config)));
    break;
  }

  freeaddrinfo(servinfo);

  if (sock_map_.size() < 1) {
    LOG_ERROR("could not bind and listen to any port!");
    throw std::exception();
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
    LOG_ERROR(std::string("poll() failed. Error: ") + std::strerror(errno));
    return 0;
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
    ISocket::status check = sock_map_.at(it->fd).handle(sock_map_, poll_list_);
    if (check == ISocket::CLOSED) {
      std::ostringstream oss;
      oss << "client: " << it->fd << " closed.";
      LOG_INFO(oss.str());
      close(it->fd);
      sock_map_.erase(it->fd);
      it = poll_list_.erase(it);
      continue;
    }
    it->revents = RESET;
    ++it;
  }
}

void Server::run() {
  while (g_signal == 0) {
    do_poll_();
    event_handler_();
  }
}
