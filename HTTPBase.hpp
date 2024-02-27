#ifndef __HTTP_BASE__
#define __HTTP_BASE__

#include <string>
#include <map>

class Socket;

class HTTPBase {
public:
  HTTPBase();
  HTTPBase(const HTTPBase &cpy);
  HTTPBase &operator=(const HTTPBase &other);
  virtual ~HTTPBase();

protected:
  void parse_buffer_();

  std::string buffer_;
  std::map<std::string, std::string> parsed_header_;
  std::string body_;

  const static size_t MAX_BUFFER = 1024;

  friend class Socket;
  friend class Server;
};

#endif // __HTTP_BASE__
