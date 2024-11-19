#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include "postgres/libpq-fe.h"

int make_read_request(char *username, char *password);

#endif