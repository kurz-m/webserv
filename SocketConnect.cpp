#include "SocketConnect.hpp"
#include <iostream>

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

void SocketConnect::handle(std::map<int, SocketInterface> &client_map,
                           std::list<pollfd_t> &poll_list) {
  (void)client_map;
  (void)poll_list;
#ifdef __verbose__
  std::cout << "handle client: " << pollfd_.fd << std::endl;
#endif
  switch (pollfd_.revents) {
  case POLLIN:
#ifdef __verbose__
    std::cout << "POLLIN!!" << std::endl;
#endif
    receive_();
    break;
  case POLLOUT:
#ifdef __verbose__
    std::cout << "POLLOUT" << std::endl;
#endif
    send_response_();
    break;
  }
}

bool SocketConnect::check_timeout() const {
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
  request_.buffer_ += std::string(buf);
  timestamp_ = std::time(NULL);
  check_recv_();
}

void SocketConnect::send_response_() {
  response_.prepare_for_send(request_);
  ssize_t num_bytes = send(pollfd_.fd, response_.buffer_.c_str(),
                           response_.buffer_.size(), MSG_DONTWAIT);
  if (num_bytes < 0) {
    throw SendRecvError();
  }
  if (static_cast<size_t>(num_bytes) < response_.buffer_.size()) {
#ifdef __verbose__
    std::cout << "server did not send the full message yet" << std::endl;
#endif
    status_ = USEND;
    pollfd_.events = POLLOUT;
  } else {
#ifdef __verbose__
    std::cout << "server did send the full message" << std::endl;
#endif
    request_.reset();
    response_.reset();
    status_ = READY;
    pollfd_.events = POLLIN;
  }
  timestamp_ = std::time(NULL);
}

void SocketConnect::check_recv_() {
  if (request_.buffer_.find("\r\n\r\n") == std::string::npos) {
#ifdef __verbose__
    std::cout << __LINE__ << "server could not find the end of the header"
              << std::endl;
#endif
    status_ = URECV;
  }
  // TODO: check content length for finished body???
  else {
#ifdef __verbose__
    std::cout << __LINE__ << "server is parsing the header" << std::endl;
#endif
    status_ = request_.parse_header();
  }
  switch (status_) {
  case READY:
    pollfd_.events = POLLOUT;
    break;
  case URECV:
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
