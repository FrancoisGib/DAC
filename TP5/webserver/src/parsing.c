#include "parsing.h"

int http_request_parse_request_line(http_request_t *http_request, char **request_ptr)
{
   char *request = *request_ptr;
   http_request->method = strtok_r(request, " ", &request);
   if (http_request->method == NULL)
   {
      printf("Invalid request received");
      return -1;
   }
   else
   {
      http_request->method = mstrdup(http_request->method);
   }

   http_request->path = strtok_r(request, " ", &request);
   if (http_request->path == NULL)
   {
      printf("Invalid request received\n");
      return -1;
   }
   else
   {
      http_request->path = mstrdup(http_request->path);
   }

   http_request->http_version = strtok_r(request, "\n", &request);
   if (http_request->http_version == NULL)
   {
      printf("Invalid request received\n");
      return -1;
   }
   else
   {
      http_request->http_version = mstrdup(http_request->http_version);
   }
   *request_ptr = request;
   return 0;
}

int http_request_parse_body(http_request_t *http_request, char **request_ptr)
{
   char *request = *request_ptr;
   int request_size = strlen(request);
   int added_body_length = MIN(request_size, http_request->content_length);
   if (http_request->body == NULL)
   {
      http_request->body = malloc(http_request->content_length + 1);
      strncpy(http_request->body, request, added_body_length);
      http_request->body[added_body_length] = '\0';
   }
   else
   {
      int body_size = strlen(http_request->body);
      int remaining_body_size = http_request->content_length - body_size;
      strncat(&http_request->body[body_size], request, remaining_body_size);
   }
   return added_body_length == http_request->content_length;
}

int http_request_parse_headers(http_request_t *http_request, char **request_ptr)
{
   char *request = *request_ptr;
   int request_size = strlen(request);

   if (request_size == 0)
   {
      printf("Empty request\n");
      return 1;
   }

   char *header_end = strstr(request, "\r\n\r\n");
   if (!header_end)
   {
      printf("Invalid request\n");
      return -1;
   }

   *header_end = '\0';
   char *body = header_end + 4; // Body pointer after "\r\n\r\n"

   char *header_str = NULL;
   while ((header_str = strtok_r(request, "\r\n", &request)) != NULL)
   {
      header_t *header = malloc(sizeof(header_t));
      char *header_name = strtok_r(header_str, ":", &header_str);
      if (header_name && *header_str == ' ')
      {
         header_str++;
      }
      header->name = mstrdup(header_name);
      header->value = mstrdup(header_str);
      if (strcmp(header_name, "Content-Length") == 0)
      {
         http_request->content_length = (int)strtol(header->value, NULL, 10);
      }
      else if (strcmp(header_name, "Referer") == 0)
      {
         char *referee_path = strchr(header->value, '/');
         referee_path += 2;
         referee_path = strchr(referee_path, '/');
         referee_path++;
         int diff_size;
         if (get_content_type_with_file_extension(referee_path) != NULL_CONTENT)
         {
            referee_path = strchr(header->value, '/');
            referee_path += 2;
            referee_path = strchr(referee_path, '/');
         }
         char *referer_directory_end_pointer = search_last_occurence(referee_path, '/');
         diff_size = referer_directory_end_pointer - referee_path;
         if (referee_path != NULL && diff_size > 0)
         {
            char *new_path = malloc(diff_size + strlen(http_request->path) + 1);
            memset(new_path, 0, diff_size + strlen(http_request->path) + 1);
            strncpy(new_path, referee_path, diff_size);
            new_path[diff_size] = '\0';

            char *rest = NULL;
            http_request->path = strtok_r(http_request->path, "?", &rest); // remove params from path (will be changed in the future to parse params differently)
            if (strncmp(new_path, http_request->path + 1, strlen(new_path)) == 0)
            {
               strcat(new_path, http_request->path + strlen(new_path) + 1);
            }
            else
            {
               strcat(new_path, http_request->path);
            }
            free(http_request->path);
            http_request->path = new_path;
         }
      }
      if (http_request->headers == NULL)
         http_request->headers = insert_in_head(http_request->headers, header);
      else
      {
         insert_in_head(http_request->headers, header);
      }
      http_request->headers_length += strlen(header->name) + strlen(header->value) + 4; // +1 to add \n\t and ": " later
   }
   *request_ptr = body;
   return *body != '\0'; // 1 if this is not the end of the headers part, 0 if this is the body or empty.
}