#ifndef __CGI_HPP__
#define __CGI_HPP__

#include "HTTPResponse.hpp"
#include <string>
#include <unistd.h>
#include <vector>

class CGI {
public:
  static std::string call_cgi(const std::string &uri,
                              const HTTPResponse &response);

private:
  CGI(const std::string &uri, const HTTPResponse &response);
  CGI(const CGI &cpy);
  CGI &operator=(const CGI &other);
  ~CGI();

  void parse_uri_();
  void execute_();
  pid_t create_pipe_();
  void prepare_env_();

  const std::string &uri_;
  const HTTPResponse &response_;
  std::string exec_;
  // std::string query_str_;
  std::vector<std::string> env_str_;
  char *env_[6];
};

#endif // __CGI_HPP__