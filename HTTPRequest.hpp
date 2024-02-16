#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

#include "HTTPBase.hpp"

class HTTPRequest : public HTTPBase {
public:
  HTTPRequest();
  HTTPRequest(const HTTPRequest &cpy);
  HTTPRequest &operator=(const HTTPRequest &other);
  ~HTTPRequest();

private:


};

#endif // __HTTPREQUEST_HPP__
