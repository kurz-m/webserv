#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

#include "HTTPBase.hpp"

class HTTPRequest : public HTTPBase {
public:
  HTTPRequest();
  HTTPRequest(const HTTPRequest &cpy);
  HTTPRequest &operator=(const HTTPRequest &other);
  ~HTTPRequest();

  void parse_header();
  void parse_body();

private:

friend class Socket;
};

#endif // __HTTPREQUEST_HPP__
