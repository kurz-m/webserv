#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse();
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void prepare_for_send();

private:
  void make_header_();
  void resolve_uri_();

  int status_code_;
  
};

#endif // __HTTP_RESPONSE_HPP__
