#include "lib.h"

char *mstrdup(const char *str)
{
   char *dup = malloc(strlen(str) + 1);
   if (dup != NULL)
   {
      strcpy(dup, str);
   }
   return dup;
}

int read_file(char buffer[MAX_FILE_READ_SIZE], char *file_path)
{
   stat_t st;
   if (stat(file_path, &st) == -1)
   {
      return -1;
   }
   if (!S_ISDIR(st.st_mode) && access(file_path, R_OK) == 0)
   {
      FILE *file = fopen(file_path, "r");
      int size = (int)fread(buffer, 1, MAX_FILE_READ_SIZE, file);
      fclose(file);
      return size;
   }
   return -1;
}

int read_file_block(char buffer[MAX_FILE_READ_SIZE], int fd, int block)
{
   return (int)pread(fd, buffer, MAX_FILE_READ_SIZE, MAX_FILE_READ_SIZE * block);
}

char *search_last_occurence(char *str, char c)
{
   int str_length = (int)strlen(str) - 1;
   for (int i = str_length; i >= 0; i--)
   {
      if (str[i] == c)
      {
         return &str[i];
      }
   }
   return NULL;
}