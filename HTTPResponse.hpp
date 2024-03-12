#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "HTTPBase.hpp"

class HTTPResponse : public HTTPBase {
public:
  HTTPResponse(const ServerBlock&);
  HTTPResponse(const HTTPResponse &cpy);
  HTTPResponse &operator=(const HTTPResponse &other);
  ~HTTPResponse();

  void prepare_for_send();

private:
  void make_header_(std::ifstream& file);
  const RouteBlock* find_route_block_() const;
  uint8_t check_uri_(const std::string &uri);

  template <typename T>
  uint8_t check_list_dir_(const T& curr_conf);


  int status_code_;

};

#endif // __HTTP_RESPONSE_HPP__
