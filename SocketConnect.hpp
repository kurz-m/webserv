#ifndef __SOCKET_CONNECT_HPP
#define __SOCKET_CONNECT_HPP

#include <list>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Socket.hpp"

/**
 * Class for a connection socket derived from Socket.
 *
 * The SocketConnect class is created by the call to accept() from the
 * SocketListen. It handles all the traffic between the webserver and a
 * connected client.
 */
class SocketConnect : public Socket {
public:

  /**
   * Derived Error class which will be thrown if send or recv fails.
   */
  class SendRecvError : public std::exception {
  public:
    const char *what() const throw();
  };

  /**
   * Parameterized constructor for the SocketConnect class.
   *
   * This constructor is used as the default constructor taking a reference to
   * its correpsonding pollfd_t element in the pollfd_list from the server and a
   * constant reference to a ServerBlock which defines all its properties.
   * Furthermore, it takes an optional timeout argument which specifies the
   * keep-alive time the socket is left open without any interaction.
   *
   * \param pollfd Reference to its corresponding pollfd element in the
   * pollfd_list of the server.
   * \param config Const reference to a ServerBlock configuration.
   * \param timeout Defines the connection timeout of the socket.
   */
  SocketConnect(pollfd_t &pollfd, const ServerBlock &config,
                int timeout = DEFAULT_TIMEOUT);

  /**
   * Copy constructor for the SocketConnect class.
   *
   * Constructor takes a const reference to another SocketConnect instance and
   * copies all the elements to a new Socket, thereby making a deep copy.
   *
   * \param other Const reference to a SocketConnect instance.
   */
  SocketConnect(const SocketConnect &cpy);

  /**
   * Copy assignment operator of the SocketConnect class.
   *
   * \param other Const reference to a SocketConnect instance.
   */
  SocketConnect &operator=(const SocketConnect &other);

  /**
   * Destructor of the SocketConnect class.
   */
  ~SocketConnect();

  /**
   * Main entry point for handling traffic between server and client.
   *
   * This function is used like a small statemachine which calls the appropriate
   * function with respect to the status_ of the SocketConnect.
   *
   * \throws std::runtime_error unexpected SocketConnect::status_ after
   * HTTPResponse::prepare_for_send().
   *
   * \param std::map<int, ISocket> Reference to the socket map of the server.
   * \param std::list<pollfd_t> Reference to the pollfd list of the server.
   * \return Status after handling the current iteration.
   */
  ISocket::status handle(std::map<int, ISocket> &sock_map,
                         std::list<pollfd_t> &poll_list);

  /**
   * Creates a new SocketConnect to the ISocket interface.
   *
   * The clone function is used to allocate memory for a new SocketConnect
   * instance which will be added into the socket map of the server.
   *
   * \return New SocketConnect instance.
   */
  SocketConnect *clone() const;

private:
  /**
   * Private member function to receive the request from the client.
   *
   * receive_ calls recv() on the client socket fd and checks if recv() was
   * successful. It also adds the amount of received bytes to the tbr_ variable
   * of the SocketConnect, adds the bytes itself to the internal buffer of it
   * and updates the timestamp_ of the SocketConnect used for the timeout check.
   * At the end it calls check_recv_ to set the following events for poll().
   *
   * \sa check_recv_()
   */
  void receive_();

  /**
   * Private member function to check the request.
   *
   * Private member function to check the received bytes and update the status_
   * of the socket. It sets the pollfd_.events according to the current status_.
   * If the socket is ready to prepare for send, it sets the poll event to
   * POLLOUT, otherwise, if the socket is not finished receiving the full
   * request, it sets the poll events to POLLIN.
   */
  void check_recv_();

  /**
   * Private member function to send the response to the connected client.
   *
   * check_recv_() is used to send the prepared response to the connected client
   * and set the status_ of the SocketConnect to determine the correct poll
   * event. If the response is not fully send, it will set the poll event to
   * POLLOUT to trigger another send event in the next poll iteration, otherwise
   * to POLLIN. Furthermore, it also distinguishes if the connection is
   * 'keep-alive' or 'close' to set the status_ accordingly.
   *
   * \throws SendRecvError() If send() has an error.
   */
  void send_response_();

  /**
   * Private member function to check the timeout of the SocketConnect.
   *
   * The purpose of this function is to check the timestamp of the socket, which
   * indicates when it was last in use and determines if the connection has
   * timed out or not.
   *
   * \return true or false regarding the timeout check.
   */
  bool check_timeout_() const;

  /**
   * Private member HTTPRequest to be able to extract elements from the request.
   *
   * This private member attribute is necessary to check the request of the
   * client during creation of the response which will later be send to the
   * client.
   */
  HTTPRequest request_;

  /**
   * Private member HTTPResponse which serves as container for the response.
   *
   * The HTTPResponse has all the necessary functions and attributes for
   * creating the appropriate response to the received request.
   */
  HTTPResponse response_;

  /**
   * Private member timeout_.
   *
   * This private member attribute is being used to specify the timeout of the
   * connection between the server and the client. It is either set within the
   * config file or falls back to the DEFAULT_TIMEOUT value.
   */
  int timeout_;

  /**
   * Private member timestamp_.
   *
   * This private member attribute serves as the timestamp on which the last
   * action happend on the instance of SocketConnect. If current time -
   * timestamp > timeout_ the connection between the client and the server will
   * be closed due to a timeout.
   */
  std::time_t timestamp_;

  friend class Server;
};

#endif /* SocketConnect.hpp */
