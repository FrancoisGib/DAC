#include "logger.h"

int write_log(char *log_message)
{
   time_t t = time(NULL);
   tm_t tm = *localtime(&t);
   stat_t st;
   if (stat("logs", &st) == -1)
   {
      if (mkdir("logs", 0700)) /* drw------- */
      {
         printf("Error creating log file");
         return -1;
      }
   }
   char time_str[64];
   if (sprintf(time_str, "logs/%d.%02d.%02d-%02d.log", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour) == EOF)
   {
      return -1;
   }

   FILE *log_file = fopen(time_str, "a");
   if (log_file != NULL)
   {
      size_t message_length = (strlen(log_message));
      int wrote_size = (int)fwrite(log_message, 1, message_length, log_file);
      return fclose(log_file) == 0 && -(wrote_size != (int)message_length);
   }
   printf("Error opening log file");
   return -1;
}

void http_request_write_log(http_request_t *http_request)
{
   int method_length = strlen(http_request->method);
   int path_length = strlen(http_request->path);
   int http_version_length = strlen(http_request->http_version);
   int additional_length = strlen("METHOD: \nPATH: \nHTTP VERSION: \nHEADERS: [\n]\nBODY:\n");
   int buffer_size = method_length + 1 + path_length + 1 + http_version_length + 1 + http_request->content_length + 1 + http_request->headers_length + additional_length;
   char buffer[buffer_size + 1];
   memset(buffer, 0, buffer_size);
   sprintf(buffer, "METHOD: %s\nPATH: %s\nHTTP VERSION: %s\nHEADERS: [\n", http_request->method, http_request->path, http_request->http_version);

   ll_node_t *header_node = http_request->headers;
   while (header_node != NULL)
   {
      header_t *header = header_node->element;
      strcat(buffer, "\t");
      strcat(buffer, header->name);
      strcat(buffer, ": ");
      strcat(buffer, header->value);
      strcat(buffer, "\n");
      header_node = header_node->next;
   }

   if (http_request->content_length > 0)
   {
      strcat(buffer, "]\nBODY:\n");
      strcat(buffer, http_request->body);
   }
   else
   {
      strcat(buffer, "]");
   }
   printf("%s\n-----------------\n", buffer);
   write_log(buffer);
   write_log("\n-----------------\n");
}