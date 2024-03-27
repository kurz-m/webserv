#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

#define BUFFER_SIZE 1024
#define CGI_TIMEOUT 25

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock &);
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  ISocket::status prepare_for_send(HTTPRequest &);
  ISocket::status check_child_status();
  ISocket::status kill_child();
  bool check_cgi_timeout();

private:
  ISocket::status status_;
  int status_code_;
  std::string root_;
  std::string uri_;
  pid_t cgi_pid_;
  int child_pipe_;
  std::time_t cgi_timestamp_;
  std::vector<pid_t> killed_childs_;

  void make_header_(const std::vector<std::string> &extra =
                        std::vector<std::string>());
  uint8_t check_uri_();
  bool check_cgi();
  void read_file_(std::ifstream &file);

  // HTTP Methods
  ISocket::status get_method_(HTTPRequest &req);
  ISocket::status post_method_(HTTPRequest& req);
  ISocket::status delete_method_();

  // cgi functionality
  ISocket::status call_cgi_(HTTPRequest &req);
  void execute_(HTTPRequest &req);
  void create_pipe_(HTTPRequest &req);
  void read_child_pipe_();

  template <typename T> uint8_t check_list_dir_(const T &curr_conf);
  template <typename T> bool check_method_(const T &curr_conf, method_e method);
  std::string create_list_dir_();

  static const int timeout_ = 10;
};

#endif // __HTTP_RESPONSE_HPP__
