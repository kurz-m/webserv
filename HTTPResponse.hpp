#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

#define BUFFER_SIZE 1024

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock &);
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  ISocket::status prepare_for_send(HTTPRequest &);
  ISocket::status check_child_status();

private:
  ISocket::status status_;
  int status_code_;
  std::string root_;
  std::string uri_;
  pid_t cgi_pid_;
  int child_pipe_;
  std::time_t child_timestamp_;

  void make_header_();
  uint8_t check_uri_();
  void read_file_(std::ifstream &file);

  // cgi functionality
  ISocket::status call_cgi_(HTTPRequest &req);
  void execute_(HTTPRequest &req);
  void create_pipe_(HTTPRequest &req);
  void read_child_pipe_();

  template <typename T>
  uint8_t check_list_dir_(const T &curr_conf);
  std::string create_list_dir_();

  static const int timeout_ = 10;
};

#endif // __HTTP_RESPONSE_HPP__
