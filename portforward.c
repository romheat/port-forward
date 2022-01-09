#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdarg.h>
#include "portforward.h"

int _verbose = 0;

void set_verbose(int setting)
{
  _verbose = setting;
}

int verbose(const char *restrict format, ...)
{
  if (!_verbose)
    return 0;

  va_list args;
  va_start(args, format);
  int ret = vprintf(format, args);
  va_end(args);

  return ret;
}

struct forward_port *create_fp_context()
{
  struct forward_port *fp = malloc(sizeof(struct forward_port));
  return fp;
};

void delete_fp_context(struct forward_port *fp)
{
  close(fp->s_client);
  close(fp->s_remote);
  close(fp->s_server);
  free(fp->host_remote);
  free(fp->remote);
  free(fp->remote_ip);
  free(fp);
}

/* simple error check */
int pass(int result, char *msg_error)
{
  if (result == -1)
  {
    fprintf(stderr, "%s\n", msg_error);
    exit(1);
  }
  return result;
}

volatile sig_atomic_t running = 1;

/* signal handler */
void sig_handler(int signo)
{
  printf("Received SIGNAL %d\n", signo);
  running = 0;
}

/* setup signals */
void setup_signals()
{
  struct sigaction signal_action;
  signal_action.sa_handler = sig_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;
  sigaction(SIGINT, &signal_action, NULL);
}

/* main listening */
void server(struct forward_port *fp)
{

  fp->s_server = pass(socket(AF_INET, SOCK_STREAM, 0), "socket error");

  bzero((char *)&fp->addr_server, sizeof(fp->addr_server));

  fp->addr_server.sin_family = AF_INET;
  fp->addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
  fp->addr_server.sin_port = htons(fp->port_server);

  int opt = 1;
  pass(setsockopt(fp->s_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "sockopt error");

  pass(bind(fp->s_server,
            (struct sockaddr *)&fp->addr_server,
            sizeof(fp->addr_server)),
       "bind server error");

  pass(listen(fp->s_server, 5), "listen error");

  printf("\n> Listen connections on %s:%d",
         inet_ntoa(fp->addr_server.sin_addr),
         fp->port_server);
}

void forward_socket(struct forward_port *fp)
{

  bzero((char *)&fp->addr_remote, sizeof(fp->addr_remote));
  fp->addr_remote.sin_family = AF_INET;
  fp->addr_remote.sin_addr = *((struct in_addr *)fp->host_remote->h_addr_list[0]);
  fp->addr_remote.sin_port = htons(fp->port_remote);

  /* establish remote socket */
  fp->s_remote = pass(socket(AF_INET, SOCK_STREAM, 0), "socket remote error");

  pass(connect(fp->s_remote,
               (struct sockaddr *)&fp->addr_remote,
               sizeof(fp->addr_remote)),
       "fordward remote connect");
}

int recv_sent(int from, int to)
{
  char buffer[BUFFER_SIZE];
  int count = recv(from, buffer, sizeof(buffer), 0);

  verbose("\nRECVC: %s", buffer);

  if (count < 0)
  {
    perror("forward traffic client");
    close(from);
    close(to);
    return -1;
  }

  if (count == 0)
  {
    close(from);
    close(to);
    return 0;
  }

  send(to, buffer, count, 0);
  return 1;
}

int forward(struct forward_port *fp)
{

  fd_set fdset;
  int maxfd = fp->s_remote > fp->s_client ? fp->s_remote : fp->s_client;

  for (;;)
  {
    FD_ZERO(&fdset);
    FD_SET(fp->s_client, &fdset);
    FD_SET(fp->s_remote, &fdset);

    pass(select(maxfd + 1, &fdset, NULL, NULL, NULL), "select error");

    /* client -> remote */
    if (FD_ISSET(fp->s_client, &fdset))
    {
      verbose("\nClient %s -> Remote %s",
              inet_ntoa(fp->addr_server.sin_addr),
              fp->remote_ip);

      if (!(recv_sent(fp->s_client, fp->s_remote) > 0))
        return 0;
    }

    /* remote -> client */
    if (FD_ISSET(fp->s_remote, &fdset))
    {
      verbose("\nRemote %s -> Client %s",
              fp->remote_ip,
              inet_ntoa(fp->addr_server.sin_addr));

      if (!(recv_sent(fp->s_remote, fp->s_client) > 0))
        return 0;
    }
  }
}

void wait_connection(struct forward_port *fp)
{
  int addr_len = sizeof(struct sockaddr_in);
  char ipstr[INET_ADDRSTRLEN];

  fp->s_client = pass(accept(fp->s_server,
                             (struct sockaddr *)&fp->addr_server,
                             (socklen_t *)&addr_len),
                      "accept error");

  strcpy(ipstr, inet_ntoa(fp->addr_server.sin_addr));
  printf("\n> Connection accepted from %s", ipstr);
}

int main(int argc, char **argv)
{

  struct forward_port *fp = create_fp_context();

  /* defaults */
  fp->port_server = 9999;
  fp->port_remote = 8080;

  int opt;
  set_verbose(0);

  /* get parameters */
  while ((opt = getopt(argc, argv, "p:s:d:v")) != -1)
  {
    switch (opt)
    {
    case 'p':
      fp->port_server = atoi(optarg);
      break;
    case 's':
      fp->remote = strdup(optarg);
      break;
    case 'd':
      fp->port_remote = atoi(optarg);
      break;
    case 'v':
      set_verbose(1);
      break;
    default:
      if (opt == 's')
        fprintf(stderr, "Option -%c requires an argument.\n", opt);
      else if (isprint(opt))
        fprintf(stderr, "Unknown option `-%c'.\n", opt);
      exit(0);
    }
  }

  if (!fp->remote)
  {
    printf("Usage: %s -p <port> -s <server> -d <destination_port> -v\n", argv[0]);
    exit(0);
  }

  /* resolve remote address */
  fp->host_remote = gethostbyname(fp->remote);
  pass(fp->host_remote != NULL, "can't resolve remote host");
  fp->remote_ip = strdup(inet_ntoa(*(struct in_addr *)fp->host_remote->h_addr_list[0]));

  printf("\n> Forward to %s > %s @ %d",
         fp->remote,
         fp->remote_ip,
         fp->port_remote);

  setup_signals();
  server(fp);

  while (running)
  {
    wait_connection(fp);
    forward_socket(fp);
    forward(fp);
  }

  delete_fp_context(fp);
  return 0;
}
