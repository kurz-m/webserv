#ifndef __HTTP_RESPONSE_HPP
#define __HTTP_RESPONSE_HPP

#include "HTTPBase.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

#define BUFFER_SIZE 1024
#define CGI_TIMEOUT 25

/**
 * Class for handling the http response to a client.
 *
 * The HTTPResponse class is part of each SocketConnect instance. It handles the
 * outgoing response to a client.
 */
class HTTPResponse : public HTTPBase {
public:
  /**
   * Parameterized constructor for the HTTPResponse class.
   *
   * This constructor is used as the default constructor of the HTTPResponse
   * class and is called when a SocketConnect is being constructed.
   *
   * \param config Const reference to a ServerBlock configuration.
   */
  HTTPResponse(const ServerBlock &);

  /**
   * Copy constructor for the HTTPResponse class.
   *
   * Constructor takes a const reference to another HTTPResponse instance and
   * copies all the elements to a new HTTPResponse instance, thereby making a
   * deep copy.
   *
   * \param cpy Const reference to a HTTPResponse instance.
   */
  HTTPResponse(const HTTPResponse &cpy);

  /**
   * Copy assignment operator of the HTTPResponse class.
   *
   * \param other Const reference to a HTTPResponse instance.
   * \return Reference to a HTTPResponse instance.
   */
  HTTPResponse &operator=(const HTTPResponse &other);

  /**
   * Destructor of the HTTPResponse class.
   */
  ~HTTPResponse();

  /**
   * Public member function for preparing the response to a client.
   *
   * This function is used to prepare the response to a specific request. It
   * incorporates also a small event handler, which checks for the requested
   * http method.
   *
   * \param req Reference to a HTTPRequest instance.
   * \return ISocket::READY_SEND after calling the appropriate functions.
   */
  ISocket::status prepare_for_send(HTTPRequest &);

  /**
   * Public member function for checking the child status of a cgi call.
   *
   * This function checks the status of a child from a cgi call. At first it
   * checks if the child timed out and if kill it. Otherwise, it handles the
   * data accordingly.
   *
   * \return ISocket::WAITCGI if still waiting for the child, otherwise
   * ISocket::READY_SEND for sending the response.
   */
  ISocket::status check_child_status();

private:
  /**
   * Private member function to create the response header.
   *
   * make_header() is always called before the response is being send to the
   * client. It will be filled with the respective elementes specified for
   * HTTP/1.1 in RFC 9110. The bare minimum it has to contain is the response
   * status line (e.g. HTTP/1.1 200 OK) + Content-Type + Content-Length. All
   * other options within the header are optional.
   *
   * \param extra Reference to a std::vector<std::string> holding extra header
   * fields. It defaults to an empty vector.
   */
  void make_header_(
      const std::vector<std::string> &extra = std::vector<std::string>());

  /** \name HTTP Methods
   * Private member functions for specific http methods.
   */
  /**@{*/

  /**
   * Private member function for handling a GET request.
   *
   * This function handles incoming GET requests from a client. With respect to
   * the configuration of autoindex, index, and the requested URI, get_method_()
   * assigns the correct body or calls CGI.
   *
   * \param req Reference to a HTTPRequest instance.
   * \return ISocket::READY_SEND or ISocket::WAITCGI.
   */
  ISocket::status get_method_(HTTPRequest &req);

  /**
   * Private member function for handling a POST request.
   *
   * This function handles incoming POST requests from a client. The function
   * then assigns the correct response body and returns the corresponding
   * status.
   *
   * \param req Reference to a HTTPRequest instance.
   * \return ISocket::READY_SEND or ISocket::WAITCGI.
   */
  ISocket::status post_method_(HTTPRequest &req);

  /**
   * Private member function for handling a DELETE request.
   *
   * This function handles incoming DELETE requests from a client. It assigns a
   * 404 status page if the file was not found or 200 if file successfully
   * deleted.
   *
   * \return ISocket::READY_SEND.
   */
  ISocket::status delete_method_();

  /**
   * Private member function for checking the allowed methods.
   *
   * This private member function is used for checking if the requested http
   * method is allowed on the specified route.
   *
   * \return true if allowed, otherwise false.
   */
  template <typename T> bool check_method_(const T &curr_conf, method_e method);
  /**@}*/

  /** \name HTTP method helper functions
   * Private member functions for helping handling the different requests.
   */
  /**@{*/

  /**
   * Private member function that reads a requested file.
   *
   * This private member function is used for reading a requested file in binary
   * representation to make it a generic function. If the file is not found, it
   * sets the status code to 404 (Not found), assigns the corresponding status
   * page and returns. Otherwise, it streams the buffer into the body of the
   * response and sets the status code to 200 (OK).
   */
  void read_file_();

  /**
   * Private member function to extract the media type from the URI.
   *
   * This private member function is used for extracting the correct media type
   * from the requested file. It makes sure to remove any unnecessary query
   * strings and defaults to application/octet-stream if nothing else was found.
   *
   * \return Returns the found media type as string or
   * "application/octet-stream"
   */
  std::string get_mime_type_();

  /**
   * Private member function to check the requested file.
   *
   * This private member function is used for checking the requested file from
   * the client. It removes all unnecessary query strings and returns an
   * uri_state enum corresponding to what file/request URI it is.
   *
   * \return To the file and request corresponding uri_state.
   */
  uint8_t check_uri_();

  /**
   * Private member function to check a CGI request.
   *
   * This private member function is used for checking of the requested call to
   * CGI can be correctly handled by the server by checking for the correct file
   * ending. Currently supported CGI calls are .py and .php.
   *
   * \return true or false
   */
  bool check_cgi_();
  /**@}*/

  /** \name CGI Functions
   * Private member functions handling CGI requests.
   */
  /**@{*/
  /**
   * Private member function for handling requests to CGI.
   *
   * This private member function is the main entry point for a CGI request. It
   * extracts the query if applicable, checks the requested CGI file and starts
   * the piping process.
   *
   * \param req Reference to a HTTPRequest instance.
   * \return ISocket::READY_SEND, or ISocket::WAITCGI if the child is not yet
   * finished executing.
   */
  ISocket::status call_cgi_(HTTPRequest &req);

  /**
   * Private member function to create the pipes for a CGI call.
   *
   * This private member function is used for synchronizing the processes betwen
   * the webserver and the child used for executing the CGI call.
   *
   * \param req Reference to a HTTPRequest instance.
   */
  void create_pipe_(HTTPRequest &req);

  /**
   * Private member function to execute the requested CGI script.
   *
   * This private member function is used for executing the CGI script that is
   * being requested by the client. It only runs within the child process.
   *
   * \param req Reference to a HTTPRequest instance.
   */
  void execute_(HTTPRequest &req);

  /**
   * Private member function to read the output of the CGI child.
   *
   * This private member function is used for reading the output created by the
   * requested CGI child and inputs it into the response body.
   */
  void read_child_pipe_();

  /**
   * Private member function to kill the child.
   *
   * This private member function is used for killing the child after it timed
   * out.
   *
   * \return ISocket::READY_SEND after killing the child.
   */
  ISocket::status kill_child_();

  /**
   * Private member function to check the timeout of the CGI child.
   *
   * This private member function is used for checking if the CGI child timed
   * out or if it is still running within the specified time.
   */
  bool check_cgi_timeout_();
  /**@}*/

  /** \name List Dir Functions
   * Private member functions for specific http methods.
   */
  /**@{*/
  /**
   * Private member function to create list dirs.
   *
   * This private member function is used for creating the directory listing
   * when autoindex on the requested route is turned on. It checks all the files
   * in the directory and sorts them folder first.
   *
   * \return Created html body for the response.
   */
  std::string create_list_dir_();

  /**
   * Private member function called within the check_uri_() function.
   *
   * This private member function is called within the check_uri_() function to
   * determine if the requested URI is configurated for listing the directory or
   * displaying the index.html file.
   *
   * \param curr_conf Reference to either a RouteBlock or a ServerBlock.
   * \return A single or combined value of uri_state.
   */
  template <typename T> uint8_t check_list_dir_(const T &curr_conf);
  /**@}*/

  /**
   * Private member attribute for the current status code of the response.
   */
  ISocket::status status_;

  /**
   * Private member attribute for the root of the server specified in the config
   * file.
   */
  std::string root_;

  /**
   * Private member attribute for the request URI.
   */
  std::string uri_;

  /**
   * Private member attribute for the child PID for a CGI call.
   */
  pid_t cgi_pid_;

  /**
   * Privat member attribute for the read end of the CGI child pipe.
   */
  int child_pipe_;

  /**
   * Private member attribute for checking CGI timeout.
   */
  std::time_t cgi_timestamp_;

  /**
   * Private member attribute for killed child PIDs.
   */
  std::vector<pid_t> killed_childs_;

  /**
   * Private member attribute for the response media type.
   */
  std::string mime_type_;

  /**
   * Private member attribute defining the connection timeout to the client.
   */
  static const int timeout_ = 10;
};

#endif /* HTTPResponse.hpp */
