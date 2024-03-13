#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"

struct cgi_containter {
  std::string exec;
  char *env[6];
};

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock &);

  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void prepare_for_send(HTTPRequest &);

private:
  void make_header_();
  uint8_t check_uri_(HTTPRequest &req);
  void read_file_(std::ifstream &file);
  int status_code_;
  std::string root_;
  std::string uri_;
  pid_t cgi_pid_;
  int child_pipe_;

  // cgi functionality
  void call_cgi_(HTTPRequest &req);
  cgi_containter prepare_env_(HTTPRequest &req);
  void execute_(HTTPRequest &req);
  std::string create_pipe_(HTTPRequest &req);

  template <typename T>
  uint8_t check_list_dir_(const T &curr_conf, HTTPRequest &req);

  std::string create_list_dir_();

  friend class CGI;
};

#endif // __HTTP_RESPONSE_HPP__
