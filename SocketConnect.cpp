#include "SocketConnect.hpp"
#include <cstring>
#include <iostream>
#include <signal.h>

SocketConnect::SocketConnect(pollfd &pollfd, const ServerBlock &config,
                             int timeout /*  = DEFAULT_TIMEOUT */)
    : Socket(pollfd, config), request_(config), response_(config),
      timeout_(timeout), timestamp_(std::time(NULL)) {}

SocketConnect::SocketConnect(const SocketConnect &cpy)
    : Socket(cpy), request_(cpy.request_), response_(cpy.response_) {
  *this = cpy;
}

SocketConnect &SocketConnect::operator=(const SocketConnect &other) {
  if (this != &other) {
    Socket::operator=(other);
    request_ = other.request_;
    response_ = other.response_;
    timeout_ = other.timeout_;
    timestamp_ = other.timestamp_;
  }
  return *this;
}

SocketConnect::~SocketConnect() {}

/// @brief handle the Socket. 2 Cases: reading data and sending data.
/// When first called, the Socket first receives the request data and processes
/// it to check, if the request is complete.
/// @param client_map unused
/// @param poll_list unused
ISocket::status SocketConnect::handle(std::map<int, ISocket> &client_map,
                                      std::list<pollfd_t> &poll_list) {
  (void)client_map;
  (void)poll_list;

  switch (status_) {
  case ISocket::PREPARE_SEND:
    status_ = response_.prepare_for_send(request_);
    if (status_ == ISocket::READY_SEND) {
      handle(client_map, poll_list);
    } else if (status_ == ISocket::WAITCGI) {
      pollfd_.events = 0;
    } else { // error
      throw std::runtime_error("Unexpected status_ after prepare_for_send()!");
    }
    break;
  case ISocket::WAITCGI:
    status_ = response_.check_child_status();
    if (status_ == ISocket::READY_SEND) {
      pollfd_.events = POLLOUT;
    } else if (status_ == ISocket::WAITCGI) {
      timestamp_ = std::time(NULL);
    }
    break;
  case ISocket::READY_RECV:
  case ISocket::URECV:
    if (pollfd_.revents & POLLIN) {
      receive_();
    } else if (check_timeout_()) {
      return ISocket::CLOSED;
    }
    break;
  case ISocket::READY_SEND:
  case ISocket::USEND:
    if (pollfd_.revents & POLLOUT) {
      send_response_();
    } else if (check_timeout_()) {
      return ISocket::CLOSED;
    }
    break;
  default:
    break;
  }
  check_poll_err_();
  return status_;
}

bool SocketConnect::check_timeout_() const {
  return std::difftime(std::time(NULL), timestamp_) > timeout_;
}

void SocketConnect::receive_() {
  ssize_t n;
  char buf[HTTPBase::MAX_BUFFER + 1] = {0};
  n = recv(pollfd_.fd, buf, HTTPBase::MAX_BUFFER, MSG_DONTWAIT);
  if (n < 0) {
    std::cerr << "recv failed!" << std::endl;
    throw std::exception();
  }
  if (n == 0) {
    status_ = ISocket::CLOSED;
    return;
  }
  // TODO: limit client max body size. maybe read header and body separately?
  while (n > 0) {
    request_.buffer_ += buf;
    std::memset(buf, 0, sizeof(buf));
    n = recv(pollfd_.fd, buf, HTTPBase::MAX_BUFFER, MSG_DONTWAIT);
  }
  timestamp_ = std::time(NULL);
  check_recv_();
}

void SocketConnect::send_response_() {
  ssize_t num_bytes = send(pollfd_.fd, response_.buffer_.c_str(),
                           response_.buffer_.size(), MSG_DONTWAIT);
  if (num_bytes < 0) {
    throw SendRecvError();
  }
  if (static_cast<size_t>(num_bytes) < response_.buffer_.size()) {
#ifdef __verbose__
    std::cout << "server did not send the full message yet" << std::endl;
#endif
    status_ = ISocket::USEND;
    pollfd_.events = POLLOUT;
  } else {
#ifdef __verbose__
    std::cout << "server did send the full message" << std::endl;
#endif
    request_ = HTTPRequest(request_.config_);
    response_ = HTTPResponse(response_.config_);
    status_ = ISocket::READY_RECV;
    pollfd_.events = POLLIN;
  }
}

void SocketConnect::check_recv_() {
  if (request_.buffer_.find("\r\n\r\n") == std::string::npos) {
#ifdef __verbose__
    std::cout << __LINE__ << "server could not find the end of the header"
              << std::endl;
#endif
    status_ = ISocket::URECV;
  }
  // TODO: check content length for finished body???
  else {
#ifdef __verbose__
    std::cout << __LINE__ << "server is parsing the header" << std::endl;
#endif
    status_ = request_.parse_header();
  }
  switch (status_) {
  case ISocket::PREPARE_SEND:
    pollfd_.events = POLLOUT;
    break;
  case ISocket::URECV:
    pollfd_.events = POLLIN;
    break;
  default:
    break;
  }
}

void SocketConnect::resolve_uri_() {}

const char *SocketConnect::SendRecvError::what() const throw() {
  return "Send or Recv returned -1!";
}

SocketConnect *SocketConnect::clone() const { return new SocketConnect(*this); }
