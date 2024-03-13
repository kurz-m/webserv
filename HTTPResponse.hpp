#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock&);
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void prepare_for_send(HTTPRequest&);

private:
  void make_header_();
  uint8_t check_uri_(HTTPRequest& req);
  void read_file_(std::ifstream& file);
  void call_cgi_(HTTPRequest& req);
  int status_code_;
  std::string root_;

  template <typename T>
  uint8_t check_list_dir_(const T& curr_conf, HTTPRequest& req);

friend class CGI;
};

#endif // __HTTP_RESPONSE_HPP__
