#ifndef __CGI_HPP__
#define __CGI_HPP__

#include "HTTPResponse.hpp"
#include <string>
#include <unistd.h>
#include <vector>

class CGI {
public:
  static std::string call_cgi(const std::string &uri,
                              const HTTPRequest &req);

private:
  CGI(const std::string &uri, const HTTPRequest &req);
  CGI(const CGI &cpy);
  CGI &operator=(const CGI &other);
  ~CGI();

  void parse_uri_();
  void execute_();
  std::string create_pipe_();
  void prepare_env_();

  const std::string &uri_;
  const HTTPRequest &req_;
  std::string exec_;
  std::vector<std::string> env_str_;
  char *env_[6];
};

#endif // __CGI_HPP__