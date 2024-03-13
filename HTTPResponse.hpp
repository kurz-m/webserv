#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock &, const HTTPRequest &);
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void prepare_for_send();

private:
  void make_header_(std::ifstream &file);
  uint8_t check_uri_(const std::string &uri);
  std::string call_cgi_();
  std::string create_list_dir_();

  const HTTPRequest &request_;
  int status_code_;

  template <typename T> uint8_t check_list_dir_(const T &curr_conf);

  friend class CGI;
};

#endif // __HTTP_RESPONSE_HPP__
