#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__

#include "HTTPBase.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse();
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

private:
  

};

#endif // __HTTPRESPONSE_HPP__