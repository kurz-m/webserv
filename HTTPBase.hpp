#ifndef __HTTP_BASE__
#define __HTTP_BASE__

#include <string>
#include <map>
#include "Settings.hpp"

class Socket;

class HTTPBase {
public:
  HTTPBase(const ServerBlock&);
  HTTPBase(const HTTPBase &cpy);
  HTTPBase &operator=(const HTTPBase &other);
  virtual ~HTTPBase();

  static const char* status_code[];
  enum method { GET, POST, DELETE, UNKNOWN };

protected:
  void parse_buffer_();

  std::string buffer_;
  std::map<std::string, std::string> parsed_header_;
  std::string body_;
  method method_;

  static std::map<int, std::string> create_map_();
  const static std::map<int, std::string> status_map_;
  const static size_t MAX_BUFFER = 1024;
  const ServerBlock& config_;  // all settings from config

friend class SocketConnect;
friend class Server;
};

#endif // __HTTP_BASE__
