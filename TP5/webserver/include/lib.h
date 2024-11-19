#ifndef LIB_H
#define LIB_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "vars.h"

#define MIN(x, y) (x) < (y) ? (x) : (y)

typedef struct stat stat_t;

typedef struct
{
   int fd;
   int block;
   char *buffer;
} read_file_parallel_arg_t;

char *mstrdup(const char *str);
int read_file(char buffer[MAX_FILE_READ_SIZE], char *file_path);
int read_file_block(char buffer[MAX_FILE_READ_SIZE], int fd, int block);
char *search_last_occurence(char *str, char c);

#endif // LIB_H