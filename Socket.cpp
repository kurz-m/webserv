#include "Socket.hpp"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdexcept>

// Socket::Socket() : pollfd_((pollfd_t){}), status_(READY), type_(UNDEFINED) {}

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



void Socket::send_response() {
  // try get location file and read and send.
  // TODO: create function for getting the correct path to the file
  // if the URI is '/' then we need to redirect it to the index.html
  // resolve_uri_();
  response_.parsed_header_.insert(std::make_pair("URI", request_.parsed_header_.at("URI")));
//   std::ifstream file( request_.parsed_header_.at("URI").c_str());
//   if (!file.is_open()) {
// #ifdef __verbose__
//   std::cout << "Could not open file. Fallback to 404 status-page" << std::endl;
// #endif
//     file.clear();
//     file.open("/home/florian/42/webserv/status-pages/404.html");
//   }
//   response_.body_.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  //file >> response_.buffer_;
  // response prepare for send;
  response_.prepare_for_send();
  ssize_t num_bytes = send(pollfd_.fd, response_.buffer_.c_str(), response_.buffer_.size(), MSG_DONTWAIT);
  if (num_bytes < 0) {
    throw SendRecvError();
    // throw an error message here and catch it in the server
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

// Socket::Socket(int sockfd, const Socket &listen_sock)
//     : type_(CONNECT), info_() {
//   socklen_t socklen = listen_sock.info_.ai_addrlen;

//   sockfd_ = accept(sockfd, listen_sock.info_.ai_addr, &socklen);
//   if (sockfd_ == -1) {
//     perror("server: accept");
//     throw std::exception();
//   }
//   events_ = POLLIN;
//   status_ = READY;
// }

// pollfd_t Socket::to_pollfd() {
//   return (pollfd_t){.fd = sockfd_, .events = events_, .revents = 0};
// }
