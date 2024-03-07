#include "Server.hpp"
#include "SocketListen.hpp"
#include "Token.hpp"
#include <cstdio>
#include <iostream>

Server::Server(const HttpBlock &config)
    : config_(config), poll_timeout_(2500) {}

Server::~Server() {}

void Server::startup() {
#ifdef __verbose__
  std::cout << "starting the server in __verbose__ mode" << std::endl;
#endif
  std::vector<ServerBlock>::const_iterator it;
  for (it = config_.servers.begin(); it != config_.servers.end(); ++it) {
    try {
      create_listen_socket_(*it);
    } catch (std::exception &e) {
      continue;
    }
  }

  // could not bind and listen to any socket.
  if (client_map_.size() < 1) {
    throw std::exception();
  }
}

void Server::create_listen_socket_(const ServerBlock &config) {
  int status;
  addrinfo_t hints;
  addrinfo_t *servinfo;
  addrinfo_t *p;

  hints.ai_family = AF_UNSPEC;     // IPv4 and IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints.ai_flags = AI_PASSIVE;     // Fill in my IP for me

  // bind to all interfaces on port
  if ((status = getaddrinfo(NULL, config.find(Token::LISTEN).str_val.c_str(),
                            &hints, &servinfo)) != 0) {
    throw std::exception();
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      continue;
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      throw std::exception();
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
    SocketListen sock(poll_list_.back(), config, *p);
    client_map_.insert(std::pair<int, Socket>(sockfd, sock));
    break;
  }

  freeaddrinfo(servinfo);
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
    try {
      SocketListen &sock = dynamic_cast<SocketListen &>(client_map_.at(it->fd));
      sock.new_connection(poll_list_, client_map_);
    } catch (std::exception &e) {
      SocketConnect &sock =
          dynamic_cast<SocketConnect &>(client_map_.at(it->fd));
      try {
        sock.handle();
      } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
        close(it->fd);
        client_map_.erase(it->fd);
        it = poll_list_.erase(it);
        continue;
      }
    }
    ++it;
  }
}

void Server::run() {
  while (true) {
    do_poll_();
#ifdef __verbose__
    std::cout << "polled" << std::endl;
#endif
    // if (num_events > 0) {
    event_handler_();
    // }
  }
}

// void Server::accept_new_connection(Socket &socket) {
//   try {
// #ifdef __verbose__
//     std::cout << "I am trying to establish a new connecion" << std::endl;
// #endif
//     int sockfd =
//         accept(socket.pollfd_.fd, servinfo_->ai_addr,
//         &servinfo_->ai_addrlen);
//     if (sockfd < 0) {
//       perror("accept");
//       return;
//     }
//     pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents =
//     0}; poll_list_.push_back(pollfd); Socket new_sock(poll_list_.back(),
//     Socket::CONNECT, config_); client_map_.insert(std::make_pair(sockfd,
//     new_sock));
//   } catch (const std::exception &e) {
//     std::cerr << e.what() << '\n';
//   }
// }

// void Server::handle_client(Socket &socket) {
// #ifdef __verbose__
//   std::cout << "handle client: " << socket.pollfd_.fd << std::endl;
// #endif
//   switch (socket.pollfd_.revents) {
//   case POLLIN:
// #ifdef __verbose__
//     std::cout << "POLLIN!!" << std::endl;
// #endif
//     socket.receive();
//     break;
//   case POLLOUT:
// #ifdef __verbose__
//     std::cout << "POLLOUT" << std::endl;
// #endif
//     socket.send_response();
//     break;
// }
// socket.pollfd_.revents = RESET;
// }
