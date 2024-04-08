#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>

#include "EventLogger.hpp"
#include "SocketConnect.hpp"

SocketConnect::SocketConnect(pollfd_t &pollfd, const ServerBlock &config,
                             int timeout /*  = DEFAULT_TIMEOUT */)
    : Socket(pollfd, config), request_(config), response_(config),
      timeout_(timeout), timestamp_(std::time(NULL)) {}

SocketConnect::SocketConnect(const SocketConnect &cpy)
    : Socket(cpy), request_(cpy.request_), response_(cpy.response_),
      timeout_(cpy.timeout_), timestamp_(cpy.timestamp_) {}

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

ISocket::status SocketConnect::handle(std::map<int, ISocket> &sock_map,
                                      std::list<pollfd_t> &poll_list) {
  (void)sock_map;
  (void)poll_list;

  switch (status_) {
  case ISocket::PREPARE_SEND:
    status_ = response_.prepare_for_send(request_);
    /* NOTE: Can only be one of the following 2 statuses */
    if (status_ == ISocket::READY_SEND) {
      handle(sock_map, poll_list);
    } else if (status_ == ISocket::WAITCGI) {
      pollfd_.events = 0;
    }
    break;
  case ISocket::WAITCGI:
    status_ = response_.check_child_status(request_);
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
      try {
        send_response_();
      } catch (std::exception &e) {
        std::ostringstream oss;
        oss << "Got an error on Sock FD: " << pollfd_.fd << " ";
        LOG_ERROR(oss.str() + e.what());
        return ISocket::CLOSED;
      }
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
  n = recv(pollfd_.fd, buf, HTTPBase::MAX_BUFFER, 0);
  if (n <= 0) {
    std::ostringstream oss;
    oss << pollfd_.fd;
    LOG_WARNING("Sock FD: " + oss.str() + " Receive Failed!");
    status_ = ISocket::CLOSED;
    return;
  }
  request_.tbr_ += n;
  request_.buffer_.append(buf, static_cast<size_t>(n));
  // LOG_DEBUG("received: " + request_.buffer_);
  timestamp_ = std::time(NULL);
  check_recv_();
}

void SocketConnect::send_response_() {
  // LOG_DEBUG("Response Buffer: " + response_.buffer_);
  ssize_t num_bytes =
      send(pollfd_.fd, response_.buffer_.c_str(), response_.buffer_.size(), 0);
  if (num_bytes < 0) {
    throw SendError();
  }
  if (static_cast<size_t>(num_bytes) < response_.buffer_.size()) {
    LOG_DEBUG("server: " + config_.server_name +
              " did not send the full message yet");
    status_ = ISocket::USEND;
    response_.buffer_ = response_.buffer_.substr(num_bytes);
    pollfd_.events = POLLOUT;
  } else {
    std::ostringstream oss;
    oss << pollfd_.fd << ". Statuscode: " << response_.status_code_;
    LOG_DEBUG("Client FD: " + oss.str() + " did send the full message");
    if (request_.keep_alive_) {
      LOG_DEBUG("keep-alive is true");
      request_ = HTTPRequest(request_.config_);
      response_ = HTTPResponse(response_.config_);
      status_ = ISocket::READY_RECV;
      pollfd_.events = POLLIN;
    } else {
      LOG_INFO("Client FD: " + oss.str() + " keep-alive is false");
      status_ = ISocket::CLOSED;
    }
  }
}

void SocketConnect::check_recv_() {
  std::ostringstream oss;
  oss << " is currently parsing the header from Client FD: ";
  oss << pollfd_.fd;
  LOG_DEBUG("Servername: " + config_.server_name + oss.str());
  status_ = request_.parse_header();
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

const char *SocketConnect::SendError::what() const throw() {
  return "Send returned -1!";
}

SocketConnect *SocketConnect::clone() const { return new SocketConnect(*this); }
