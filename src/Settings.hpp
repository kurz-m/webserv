#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Token.hpp"

#define DEFAULT_TIMEOUT 25

struct HttpBlock;
struct ServerBlock;

/**
 * Exception for when there is a wrong setting in the Parser.
 */
class NotFoundError : public std::runtime_error {
public:
  NotFoundError(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * Struct that contains the settings for a route/location block in the config.
 *
 * The RouteBlock defines an endpoint within a ServerBlock. It is possible to
 * have multiple RouteBlocks within a ServerBlock.
 */
typedef struct RouteBlock {
  /**
   * Parameterized constructor for the RouteBlock.
   *
   * This constructor is taking a const reference to a ServerBlock in order to
   * pass down the already defined settings within the server. If there is no
   * setting set within the route, then the server settings will be taken.
   * Otherwise, settings withing the route superseed the inherited settings.
   *
   * \param server Const reference to a ServerBlock.
   */
  RouteBlock(const ServerBlock &server);

  /**
   * A public variable of the RouteBlock.
   *
   * Path is the endpoint of the specified. This will be parsed as URI.
   */
  std::string path;

  /**
   * A public variable of the RouteBlock.
   *
   * Allow configurations which http methods are allowed on the specified URI.
   */
  int allow;

  /**
   * A public variable of the RouteBlock.
   *
   * Autoindex specifies if the URI allows directory listing. This will be
   * decided base on the interaction with other settings.
   */
  int autoindex;

  /**
   * A public variable of the RouteBlock.
   *
   * Index specifies the index.html page for the given route.
   */
  std::string index;

  /**
   * A public variable of the RouteBlock.
   *
   * Redir specifies if a specific URI should redirect to a given website.
   */
  std::string redir;
} RouteBlock;

/**
 * Struct that contains the settings for a server block in the config.
 *
 * The ServerBlock is a single server within the webserv program. It is possible
 * to have multiple ServerBlocks within a HttpBlock.
 */
typedef struct ServerBlock {
  ServerBlock(const HttpBlock &http);

  /**
   * Public helper function to find a RouteBlock within a server.
   *
   * The find() function is used for finding a RouteBlock within a server by
   * comparing the input uri against the RouteBlock::path variable within each
   * element of the routes member attribute.
   *
   * \param uri URI that needs to be searched.
   * \return A pointer to a routeblock or NULL if not found, respectively.
   */
  const RouteBlock *find(const std::string &uri) const;

  /**
   * Public vector of RouteBlocks.
   *
   * Each server can have multiple routes which will be stored within the routes
   * member attribute of the ServerBlock struct.
   */
  std::vector<RouteBlock> routes;

  /**
   * Public variable of the ServerBlock.
   *
   * Allow configurations which http methods are allowed on the specified URI.
   */
  int allow;

  /**
   * Public variable of the ServerBlock.
   *
   * Autoindex specifies if the URI allows directory listing. This will be
   * decided base on the interaction with other settings.
   */
  int autoindex;

  /**
   * Public variable of the ServerBlock.
   *
   * Client max body size specifies the maximum size of the body a request can
   * send to the webserver.
   */
  size_t client_max_body_size;

  /**
   * Public variable of the ServerBlock.
   *
   * Default type specifies the type that is being returned to the client within
   * the response when nothing other is specified.
   */
  std::string default_type;

  /**
   * Public variable of the ServerBlock.
   *
   * Index specifies the index.html page for the given route.
   */
  std::string index;

  /**
   * Public variable of the ServerBlock.
   *
   * Keepalive timeout specifies the time a connection to a client is kept open
   * before it is being closed. This applies by default as stated in HTTP/1.1
   * but is overwritten when the request sends a Connection: close within the
   * header.
   */
  int keepalive_timeout;

  /**
   * Public variable of the ServerBlock.
   *
   * Listen specified the port number the server should listen on.
   * The ports 0 - 1023 are reserved system ports.
   */
  std::string listen;

  /**
   * Public variable of the ServerBlock.
   *
   * Root specifies the webroot which will be prepended to the URI in order to
   * find resources of the webserver.
   */
  std::string root;

  /**
   * Public variable of the ServerBlock.
   *
   * Server name is the name given to the specific ServerBlock.
   */
  std::string server_name;
} ServerBlock;

/**
 * Struct that contains the settings for a http block in the config.
 *
 * The HttpBlock is a unique block within the configuration file. It is the
 * outer most block and can contain multiple server blocks and also already
 * define some default values for the servers.
 */
typedef struct HttpBlock {
  /**
   * Default constructor for the HttpBlock.
   *
   * This constructor creates a HttpBlock with default values, which will be
   * overwritten from the parsed configuration file..
   */
  HttpBlock();

  /**
   * Public vector of ServerBlocks.
   *
   * A http block can have multiple servers which will be stored within the
   * servers member attribute of the HttpBlock struct.
   */
  std::vector<ServerBlock> servers;

  /**
   * Public variable of the HttpBlock.
   *
   * Client max body size specifies the maximum size of the body a request can
   * send to the webserver.
   */
  int client_max_body_size;

  /**
   * Public variable of the HttpBlock.
   *
   * Default type specifies the type that is being returned to the client within
   * the response when nothing other is specified.
   */
  std::string default_type;

  /**
   * Public variable of the HttpBlock.
   *
   * Keepalive timeout specifies the time a connection to a client is kept open
   * before it is being closed. This applies by default as stated in HTTP/1.1
   * but is overwritten when the request sends a Connection: close within the
   * header.
   */
  int keepalive_timeout;

  /**
   * Public variable of the HttpBlock.
   *
   * Root specifies the webroot which will be prepended to the URI in order to
   * find resources of the webserver.
   */
  std::string root;

} HttpBlock;
/**@}*/

#endif /* Settings.hpp */
