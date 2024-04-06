#ifndef __HTTP_BASE_HPP
#define __HTTP_BASE_HPP

#include <map>
#include <string>

#include "HTTP.hpp"
#include "Settings.hpp"
#include "Socket.hpp"

#define DCRLF "\r\n\r\n"
#define CRLF "\r\n"
#define CR "\r"

/**
 * A base class for HTTPRequest and HTTPResponse.
 *
 * This class serves as a base class for the response and request class. It
 * holds the member attributes shared by the two http types. Furthermore, it
 * holds static member maps, which can be accessed by every instace to make the
 * appropriate response or parse the appropriate request, respectively.
 */
class HTTPBase {
public:
  /**
   * Parameterized constructor for the HTTPBase class.
   *
   * This constructor is taking a reference to a Serverblock specifying all
   * configurations from the configuration file.
   *
   * \param config Reference to a ServerBlock.
   */
  HTTPBase(const ServerBlock &);

  /**
   * Copy constructor for the HTTPBase class.
   *
   * Constructor takes a const reference to another HTTPBase instance and copies
   * all the elements to a new HTTPBase instance, thereby making a deep copy.
   *
   * \param other Const reference to a HTTPBase.
   */
  HTTPBase(const HTTPBase &cpy);

  /**
   * Copy assignment operator of the HTTPBase class.
   *
   * \param other Const reference to a HTTPBase.
   */
  HTTPBase &operator=(const HTTPBase &other);

  /**
   * Destructor of the HTTPBase class virtual.
   *
   * Define the destructor of the base class as virtual to let the derived
   * classes define it themselves.
   */
  virtual ~HTTPBase();

  /**
   * Public member function to clear the member attributes.
   *
   * This public member function clears the buffer_, body_, header_, and
   * status_code_ before handling a new request. This is necessery due to the
   * keep-alive connection type of HTTP/1.1
   */
  void reset();

  /**
   * Public member function to create the respective status html page.
   *
   * This public member function is used to create the correct status html page
   * as std::string for the given status code of the response.
   *
   * \param status_code The status code of the response.
   * \param message Message to be inserted into the response page. Defaults to
   * empty.
   * \return The created status html response body.
   */
  std::string create_status_html(int status_code,
                                 const std::string &message = "") const;

protected:
  /**
   * Protected member attribute to store data within the request and response.
   *
   * This buffer serves different purpose with respect to the request and
   * response class. Within the HTTPRequest class, it is used to store the bytes
   * from the recv() class and within the HTTPResponse class, it is used for
   * storing the header that is being send.
   */
  std::string buffer_;

  /**
   * Protected member attribute that stores the body from the response or
   * request, respectively.
   */
  std::string body_;

  /**
   * Protected member attribute that stores the header received from the request
   * or created in the response, respectively.
   */
  std::string header_;

  /**
   * Protected member attribute that stores the status code of the
   * request/response.
   */
  int status_code_;

  /**
   * Protected member attribute for a ServerBlock from the configuration file.
   *
   * This protected member is a const reference to the ServerBlock parsed during
   * startup of the server. It holds all necessary settings.
   *
   * \sa ServerBlock
   */
  const ServerBlock &config_;

  /**
   * Protected member attribute to hold necessary status codes.
   *
   * This map contains all necessary status codes for HTTP/1.1 that we
   * implemented in the webserver. It takes an int as key and returns the
   * respective status line as a string.
   */
  const static std::map<int, std::string> status_map_;

  /**
   * Protected const static member attribute to hold necessary media types.
   *
   * This map contains all necessary media types for HTTP/1.1 that we
   * implemented in the webserver. It takes an int as key and returns the
   * respective media type as a string.
   */
  const static std::map<std::string, std::string> mime_map_;

  /**
   * Protected member attribute to declare the max buffer for recv.
   *
   * Currently, this is set to 128KB but can be changed if necessary.
   */
  const static size_t MAX_BUFFER = 32 * 4096;

  friend class SocketConnect;
  friend class Server;
};

#endif /* HTTPBase.hpp */
