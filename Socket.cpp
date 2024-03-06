#include "Socket.hpp"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdexcept>

Socket::Socket(pollfd &pollfd, type type, int timeout /* = DEFAULT_TIMEOUT*/)
    : pollfd_(pollfd), status_(READY), type_(type), timeout_(timeout) {
  timestamp_ = std::time(NULL);
}

Socket::Socket(const Socket &other) : pollfd_(other.pollfd_) { *this = other; }

Socket &Socket::operator=(const Socket &other) {
  if (this != &other) {
    request_ = other.request_;
    response_ = other.response_;
    pollfd_ = other.pollfd_;
    status_ = other.status_;
    type_ = other.type_;
    timeout_ = other.timeout_;
    timestamp_ = other.timestamp_;
  }
  return *this;
}

void Socket::check_recv_() {
  if (request_.buffer_.find("\r\n\r\n") == std::string::npos) {
#ifdef __verbose__
  std::cout << __LINE__ << "server could not find the end of the header" << std::endl;
#endif
    status_ = URECV;
    pollfd_.events = POLLIN;
  }
  // TODO: check content length for finished body???
  else {
#ifdef __verbose__
  std::cout << __LINE__ << "server is parsing the header" << std::endl;
#endif
    request_.parse_header(*this);
    status_ = READY;
  }
  // Set events_ according to status_
  if (status_ == READY) {
#ifdef __verbose__
  std::cout << __LINE__ << "server is ready to send the response" << std::endl;
#endif
    pollfd_.events = POLLOUT;
  }
}

void Socket::receive() {
  ssize_t n;
  char buf[HTTPBase::MAX_BUFFER + 1] = {0};
  n = recv(pollfd_.fd, buf, HTTPBase::MAX_BUFFER, MSG_DONTWAIT);
  if (n < 0) {
    std::cerr << "recv failed!" << std::endl;
    throw std::exception();
  }
  request_.buffer_ += std::string(buf);
  timestamp_ = std::clock();
  check_recv_();
}

void Socket::interpret_request_headers_() {
  // try get location file and read and send.
  // TODO: create function for getting the correct path to the file
  // if the URI is '/' then we need to redirect it to the index.html
  // also check permission for the specified URI
  response_.parsed_header_.insert(std::make_pair("URI", request_.parsed_header_.at("URI")));
}

void Socket::send_response() {
  interpret_request_headers_();
  response_.prepare_for_send();
  ssize_t num_bytes = send(pollfd_.fd, response_.buffer_.c_str(), response_.buffer_.size(), MSG_DONTWAIT);
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
    status_ = READY;
    pollfd_.events = POLLIN;
  }
  timestamp_ = std::clock();
}

const char* Socket::SendRecvError::what() const throw()
{
  return "Send or Recv returned -1!";
}

Socket::~Socket() {}
