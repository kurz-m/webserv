#ifndef __HTTPREQUEST_HPP
#define __HTTPREQUEST_HPP

#include "HTTPBase.hpp"
#include "Settings.hpp"
#include "Socket.hpp"

/**
 * Class for handling the http request from a client.
 *
 * The HTTPRequest class is part of each SocketConnect instance. It handles the
 * incoming request from a client, parses the header of the request, as well as
 * the body.
 */
class HTTPRequest : public HTTPBase {
public:
  /**
   * Parameterized constructor for the HTTPRequest class.
   *
   * This constructor is used as the default constructor of the HTTPRequest
   * class and is called when a SocketConnect is being constructed.
   *
   * \param config Const reference to a ServerBlock configuration.
   */
  HTTPRequest(const ServerBlock &);

  /**
   * Copy constructor for the HTTPRequest class.
   *
   * Constructor takes a const reference to another HTTPRequest instance and
   * copies all the elements to a new HTTPRequest instance, thereby making a
   * deep copy.
   *
   * \param cpy Const reference to a HTTPRequest instance.
   */
  HTTPRequest(const HTTPRequest &cpy);

  /**
   * Copy assignment operator of the HTTPRequest class.
   *
   * \param other Const reference to a HTTPRequest instance.
   */
  HTTPRequest &operator=(const HTTPRequest &other);

  /**
   * Destructor of the HTTPRequest class.
   */
  ~HTTPRequest();

  /**
   * Main interface for handling an incoming request from a client.
   *
   * This function is used to parse the header of an incoming request from a
   * client into the parsed header variable. It also determines if the
   * connection is set to close or keep-alive.
   *
   * \return ISocket::URECV if header or body is not fully received or
   * ISocket::PREPARE_SEND if both body and header are fully received and
   * parsed.
   */
  ISocket::status parse_header();

private:
  /**
   * Private member function to parse the body of a request.
   *
   * This function is used to parse the body of an incoming request from a
   * client into the protected member variable body_.
   */
  void parse_body_();

  /**
   * Private member attribute holding key/value pairs of the parsed header.
   *
   * This private member attribute is used for storing the key/value pairs
   * received in the request header from the client.
   */
  std::map<std::string, std::string> parsed_header_;

  /**
   * Private member attribute defining the http method of the request.
   */
  method_e method_;

  /**
   * Private member attribute counting the received bytes from recv().
   */
  size_t tbr_;

  /**
   * Private member attribute of the HTTPRequest class.
   *
   * This private member attribute bool displays if the request header is fully
   * parsed or not.
   */
  bool header_parsed_;

  /**
   * Private member attribute of the HTTPRequest class.
   *
   * This private member attribute bool defines if the connection from the
   * server to the client is kept open due to the keep-alive option or set to
   * false if the header sets the connection to close.
   */
  bool keep_alive_;

  friend class SocketConnect;
  friend class HTTPResponse;
};

#endif /* HTTPRequest.hpp */
