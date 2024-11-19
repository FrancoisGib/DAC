#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "http.h"

typedef struct tm tm_t;
typedef struct stat stat_t;

struct http_request_s;
typedef struct http_request_s http_request_t; // Forward declaration

int write_log(char *message);
void http_request_write_log(http_request_t *http_request);

#endif // LOGGER_H