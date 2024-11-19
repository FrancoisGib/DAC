#ifndef HTTP_H
#define HTTP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "lib.h"
#include "http_tree.h"
#include "logger.h"
#include "vars.h"
#include "http_structs.h"
#include "parsing.h"

#ifdef USE_SSL
#include "ssl.h"
#endif

typedef struct sockaddr_in sockaddr_in_t;

void start_server(int port);

#endif // HTTP_H
