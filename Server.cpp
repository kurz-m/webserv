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

// bool Server::bind_sock(std::map<int, Socket>::iterator &it) {
//   if (bind(it->first, it->second.info_.ai_addr, it->second.info_.ai_addrlen) ==
//       -1) {
//     perror("server: bind");
//     close(it->first);
//     client_map_.erase(it);
//     return false;
//   }
//   if (listen(it->first, 10)) {
//     perror("server: listen");
//     close(it->first);
//     client_map_.erase(it);
//     return false;
//   }
//   it->second.events_ = POLLIN;
//   pollfds_.push_back(it->second.to_pollfd());
//   return true;
// }

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

  // bool ipv6found = false;

  // for (std::map<int, Socket>::iterator it = client_map_.begin();
  //     it != client_map_.end(); it++) {
  //   if (!ipv6found && it->second.info_.ai_family == PF_INET6) {
  //     int no = 0;
  //     if (setsockopt(it->first, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) ==
  //         -1) {
  //       perror("Could not set IPV6_V6ONLY to false");
  //       continue;
  //     }
  //     if (bind_sock(it)) {
  //       ipv6found = true;
  //       Socket sock = it->second;
  //       client_map_.clear();
  //       client_map_[sock.sockfd_] = sock;
  //       break;
  //     }
  //   }
  // }

  // if (!ipv6found) {
  //   for (std::map<int, Socket>::iterator it = client_map_.begin();
  //       it != client_map_.end(); it++) {
  //     if (bind_sock(it)) {
  //       Socket sock = it->second;
  //       client_map_.clear();
  //       client_map_[sock.sockfd_] = sock;
  //       break;
  //     }
  //   }
  // }

  // could not bind and listen to any socket.
  if (client_map_.size() < 1) {
    throw std::exception();
  }
}

void Server::prepare_poll_() {
  pollfds_.clear();
  std::list<pollfd_t>::iterator it;
  for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
    pollfds_.push_back(*it);
  }
}

void Server::copy_revents_()
{
  int i = 0;
  std::list<pollfd_t>::iterator it;
  for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
    it->revents = pollfds_[i++].revents;
  }
}

void Server::run() {
  int num_events;

  while (true) {
    prepare_poll_();
    num_events = poll(&pollfds_[0], pollfds_.size(), timeout_);
    if (num_events < 0) {
      perror("server: poll");
      throw std::exception();
    }
    std::cout << "polled" << std::endl;
    // sleep(1);
    if (num_events > 0) {
      copy_revents_();
      std::list<pollfd_t>::iterator it;
      for (it = poll_list_.begin(); it != poll_list_.end(); ++it) {
      // for (size_t i = 0; i < pollfds_.size(); i++) {
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
  }
}

void Server::accept_new_connection(Socket &socket) {
  try {
    std::cout << "I am trying to establish a new connecion" << std::endl;
    int sockfd = accept(socket.pollfd_.fd, servinfo_->ai_addr, &servinfo_->ai_addrlen);
    pollfd_t pollfd = (pollfd_t){.fd = sockfd, .events = POLLIN, .revents = 0};
    poll_list_.push_back(pollfd);
    Socket new_sock(poll_list_.back(), Socket::CONNECT);
    client_map_.insert(std::make_pair(sockfd, new_sock));
    // client_map_[sockfd] = Socket(poll_list_.back(), Socket::CONNECT);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

void Server::handle_client(Socket &socket) {
  std::cout << "handle client: " << socket.pollfd_.fd<< std::endl;
  // ssize_t n;
  // char buf[Socket::MAX_BUFFER + 1] = {0};
  switch (socket.pollfd_.revents) {
  case POLLIN:
    // n = recv(socket.sockfd_, &socket.buffer_[0], Socket::MAX_BUFFER - 1,
    //         MSG_DONTWAIT);
		// (void)n;
    // socket.buffer_ += std::string(buf);
    socket.receive();
    if (socket.status_ == Socket::READY) {
      std::cout << socket.request_.buffer_ << std::endl;
    }
    break;
  case POLLOUT:
    std::string buffer = "HTTP/1.1 200 OK\r\nContent-Length: 26\r\n\r\n<html>Hello, World!</html>"; // 
    send(socket.pollfd_.fd, buffer.c_str(), buffer.size(), 0);
    socket.pollfd_.events = POLLIN;
    break;
  }
}
