#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

int main(void) {
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo, *p;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints.ai_flags = AI_PASSIVE; // Fill in my IP for me

  if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
  }

  int len = 0;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    void *addr;
    std::string ipver;

    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    std::cout << inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr) << std::endl;

    len++;
  }

  std::cout << "len = " << len << std::endl;

  freeaddrinfo(servinfo);
  return 0;
}