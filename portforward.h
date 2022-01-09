#ifndef PORTFORWARD_H
#define PORTFORWARD_H

#include <arpa/inet.h>
#include <netdb.h>

struct forward_port
{
  int s_server;
  int s_remote;
  int s_client;

  struct sockaddr_in addr_server;
  int port_server;

  struct sockaddr_in addr_remote;
  int port_remote;

  struct hostent *host_remote;
  char *remote;
  char *remote_ip;
};

#define BUFFER_SIZE 1024 * 8

#endif