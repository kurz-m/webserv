#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse();
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void make_response();

private:
  
};

#endif // __HTTP_RESPONSE_HPP__
