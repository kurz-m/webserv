#include "SocketConnect.hpp"
#include <iostream>
#include <cstring>

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
void SocketConnect::handle(std::map<int, SocketInterface> &client_map,
                           std::list<pollfd_t> &poll_list) {
  (void)client_map;
  (void)poll_list;
  // check the State here. If state == WATICGI, set the events to 0
  // since we dont want to send anything just yet? see Note below.  
  // Set the events according to the State, then wait one more poll and then send.
  // maybe we need something like an incoming status and a result status,
  // a before and after handle status.
  // Note: if we set the events to 0 during WAITCGI, we will wait up to 
  // poll_timeout_ after child finishes, until we actually send the data. Maybe
  // don't set events to 0?
  // If we had threads this would be a good idea. since we dont have threads
  // leave the continuous polling in maybe. TBD.
  if (status_ == WAITCGI) {
    status_ = response_.check_child_status();
    if (status_ == WAITCGI) {
      pollfd_.events = 0;
    }
  }
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
    respond_();
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
  if (n == 0) {
    status_ = CLOSED;
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
}



void SocketConnect::respond_() {
  switch (status_) {
  case READY:
    status_ = response_.prepare_for_send(request_);
    break;
  case WAITCGI:
    status_ = response_.check_child_status();
    break;
  default:
    break;
  }
  if (status_ == READY | status_ == USEND) {
    send_response_();
  }
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
