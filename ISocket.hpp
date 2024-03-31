#ifndef __SOCKET_INTERFACE_HPP
#define __SOCKET_INTERFACE_HPP

#include <list>

#include "Settings.hpp"

typedef struct addrinfo addrinfo_t;
typedef struct pollfd pollfd_t;

#define DEFAULT_TIMEOUT 15

class Socket;

/**
 * Interface class for SocketListen and SocketConnect.
 *
 * This class serves as interface for our 2 different types of Sockets.
 * Within the fd/Socket map, ISocket is being used to avoid the necessity for
 * different socket types and therefore using a branch to choose which handle
 * function has to be called. The only member attribute it has is a Pointer to a
 * base Socket instance which could be either SocketListen or SocketConnect. New
 * is being called upon creating of the new ISocket and is bound to the lifetime
 * of the ISocket to avoid memory leaks and a manual call to delete (which is
 * being called within the destructor).
 */
class ISocket {
public:
  /**
   * Need this here because we need it here.
   *
   * Enum for displaying the current status of the socket. This enums are used
   * in our SocketConnect statemachine to chose the correct event.
   */
  enum status {
    PREPARE_SEND, /**<ConnectSocket is ready for preparing the response*/
    READY_SEND,   /**<ConnectSocket is ready to send the response*/
    READY_RECV,   /**<Listen: ready to accept connection.
                      Connect: ready to receive a request.*/
    URECV,        /**<ConnectSocket has not yet fully received the request*/
    USEND,        /**<ConnectSocket has not yet fully send the response*/
    WAITCGI,      /**<ConnectSocket is waiting for the CGI child*/
    CLOSED,       /**<Socket is being closed for multiple reasons
                      Connect -> timeout, Listen -> connection error*/
  };

  /**
   * Parameterized constructor for the ISocket class.
   *
   * This constructor serves as interface for creating a SocketListen instance.
   * This is possible due to the overload with different input arguments.
   *
   * \sa SocketListen
   *
   * \param pollfd Reference to its corresponding pollfd element in the
   * pollfd_list of the server.
   * \param config Const reference to a ServerBlock configuration.
   * \param info Const reference to a addrinfo_t struct.
   */
  ISocket(pollfd &pollfd, const ServerBlock &config, const addrinfo_t &info);

  /**
   * Parameterized constructor for the ISocket class.
   *
   * This constructor serves as interface for creating a SocketConnect instance.
   * This is possible due to the overload with different input arguments.
   *
   * \sa SocketConnect
   *
   * \param pollfd Reference to its corresponding pollfd element in the
   * pollfd_list of the server.
   * \param config Const reference to a ServerBlock configuration.
   * \param timeout Defines the connection timeout of the socket.
   */
  ISocket(pollfd_t &pollfd, const ServerBlock &config,
          int timeout = DEFAULT_TIMEOUT);

  /**
   * Copy constructor for the ISocket class.
   *
   * Constructor takes a const reference to another ISocket instance and copies
   * all the elements to a new Socket, thereby making a deep copy.
   *
   * \param other Const reference to a ISocket instance.
   */
  ISocket(const ISocket &obj);

  /**
   * Copy assignment operator of the ISocket class.
   *
   * \param other Const reference to a ISocket instance.
   */
  ISocket &operator=(const ISocket &obj);

  /**
   * Destructor of the ISocket class.
   *
   * This constructor calls delete on its private member sock_ in order to free
   * the memory allocated during construction.
   */
  ~ISocket();

  /**
   * Interface function for SocketConnect and SocketListen.
   *
   * \sa SocketConnect::handle()
   * \sa SocketListen::handle()
   */
  status handle(std::map<int, ISocket> &sock_map,
                std::list<pollfd_t> &poll_list);

  /**
   * Interface function for SocketConnect and SocketListen.
   *
   * \sa SocketConnect::check_timeout_()
   * \sa SocketListen::check_timeout_()
   */
  bool check_timeout_() const;

private:
  /**
   * Pointer to a base Socket instance.
   *
   * This private member attribute is a pointer to a base Socket instance which
   * makes it possible to either hold a SocketConnect or SocketListen instance.
   * Together with the interface fucntions, this enables the webserver to just
   * call handle() on its sock_ member and with respect to the derived class
   * calls the right function.
   */
  Socket *sock_;

  friend class Server;
};

#endif /* ISocket.hpp */
