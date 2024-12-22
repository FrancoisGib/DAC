#include "http.h"

int sock = -1;
int nb_processes = NB_PROCESSES;
tree_t *http_tree;
endpoint_t *error_endpoint;
pid_t parent_pid;

#ifdef USE_SSL
#define SOCK ssl

SSL_CTX *ctx = NULL;
SSL *ssl = NULL;

int ssl_read(SSL *s, char buffer[MAX_REQUEST_SIZE], int buf_size)
{
   if (s == NULL)
   {
      return -1;
   }

   if (SSL_accept(s) <= 0)
   {
      ERR_print_errors_fp(stderr);
      return -1;
   }
   else
   {
      int size = 0;
      if ((size = SSL_read(s, buffer, buf_size)) <= 0)
      {
         ERR_print_errors_fp(stderr);
         return -1;
      };
      return size;
   }
}

#define READ ssl_read
#define WRITE SSL_write

#else
#define SOCK client_socket

#define READ read
#define WRITE write
#endif

void sigint_handler(int code)
{
   printf("Process %d ended\n", getpid());
#ifdef USE_SSL
   SSL_CTX_free(ctx);
#endif
   if (sock != -1)
   {
      close(sock);
   }
   if (http_tree != NULL)
   {
      free_http_tree(http_tree);
   }
   if (getpid() == parent_pid)
   {
#ifdef USE_SSL
      printf("\nFreed SSL context");
#endif
      printf("\nClosed socket");
      printf("\nFreed http tree\n");
   }
   exit(0);
}

int http_response_build_type_et_file(http_response_t *http_response, endpoint_t *endpoint)
{
   stat_t st;
   if (stat(endpoint->response.resource.content, &st) == -1)
   {
      return -1;
   }
   else
   {
      http_response->resource.file_path = endpoint->response.resource.content;
      http_response->content_type = get_content_type_with_file_extension(http_response->resource.file_path);
      http_response->content_length = (int)st.st_size;
      return 0;
   }
}

void http_response_build_type_et_text(http_response_t *http_response, endpoint_t *endpoint, char response_buffer[MAX_RESPONSE_SIZE])
{
   strcpy(response_buffer, endpoint->response.resource.content);
   http_response->content_length = strlen(endpoint->response.resource.content);
}

void http_response_build_type_et_func(http_req_res_t *http_req_res, endpoint_t *endpoint, char response_buffer[MAX_RESPONSE_SIZE])
{
   resource_function function = endpoint->response.resource.function;
   function(http_req_res);
   if (http_req_res->response->content_length > 0)
   {
      strcpy(response_buffer, http_req_res->response->resource.content);
   }
}

int http_response_build_type_et_directory(http_response_t *http_response, http_request_t *http_request, endpoint_t *endpoint)
{
   char *path_ptr = http_request->path;
   int size = strlen(path_ptr);
   if (strlen(endpoint->path) > 0) // if path is not /
   {
      while (size > 0 && strcmp(path_ptr, endpoint->path) != 0)
      {
         path_ptr++;
         size--;
      }
   }
   path_ptr += strlen(endpoint->path) + 1;
   char *file_path = malloc(strlen(path_ptr) + strlen(endpoint->response.resource.content) + 2); // pass the \0 made by get_endpoint
                                                                                                 // add 1 more if there's no / at the end of the directory path
   strcpy(file_path, endpoint->response.resource.content);
   int file_path_size = (int)strlen(endpoint->response.resource.content);
   file_path[file_path_size] = '/'; // always / at the end
   file_path[file_path_size + 1] = '\0';
   strcat(file_path, path_ptr);
   http_response->resource.file_path = file_path;
   http_response->content_type = get_content_type_with_file_extension(http_response->resource.file_path);
   stat_t st;
   stat(file_path, &st);
   if (stat(file_path, &st) != -1 && !S_ISDIR(st.st_mode))
   {
      http_response->content_length = (int)st.st_size;
      return 0;
   }

   free(file_path);
   return -1;
}

void http_response_send_et_file(http_response_t *http_response, char response_buffer[MAX_RESPONSE_SIZE], int client_socket)
{
   int response_length = strlen(response_buffer);
   if (http_response->content_type == IMAGE_X_ICON || http_response->content_type == IMAGE_PNG)
   {
      char buf[http_response->content_length + 1];
      memset(buf, 0, http_response->content_length);
      int read_size;
      if (*http_response->resource.file_path == '\0')
      {
         printf("Resource path error\n");
         return;
      }
      if ((read_size = read_file(buf, http_response->resource.file_path)) != -1)
      {
         strncat(response_buffer, buf, read_size);
         int written_size = WRITE(SOCK, buf, response_length + read_size);
         if (written_size != response_length + read_size)
         {
            printf("Error sending file\n");
         }
      }
      else
      {
         printf("Error reading file\n");
      }
   }
   else
   {
      if (response_length + http_response->content_length < MAX_RESPONSE_SIZE && http_response->content_length <= MAX_FILE_READ_SIZE)
      {
         if (*http_response->resource.file_path == '\0')
         {
            printf("Resource path error\n");
            return;
         }
         char buf[http_response->content_length];
         memset(buf, 0, http_response->content_length);
         int read_size;
         if ((read_size = read_file(buf, http_response->resource.file_path)) != -1)
         {
            strncat(response_buffer, buf, read_size);
            int written_size = WRITE(SOCK, response_buffer, response_length + read_size);
            if (written_size != response_length + read_size)
            {
               printf("Error sending file\n");
            }
         }
         else
         {
            printf("Error reading file\n");
         }
      }
      else
      {
         int written_size = WRITE(SOCK, response_buffer, response_length);
         if (written_size != response_length)
         {
            printf("Error sending file\n");
         }
         char buf[MAX_FILE_READ_SIZE];
         memset(buf, 0, MAX_FILE_READ_SIZE);
         int read_size;
         int fd = open(http_response->resource.file_path, O_RDONLY);
         if (fd != -1)
         {
            int block = 0;
            while ((read_size = read_file_block(buf, fd, block)) > 0)
            {
               written_size += WRITE(SOCK, buf, read_size);
               block++;
               memset(buf, 0, read_size);
            }
            close(fd);
            if (written_size != http_response->content_length + response_length)
            {
               printf("Error splitting response data into chunks\n");
            }
         }
      }
   }
}

void http_response_build_by_type(http_req_res_t *http_req_res, endpoint_t *endpoint, char response_content_buffer[MAX_RESPONSE_SIZE], http_request_t *http_request)
{
   http_response_t *http_response = http_req_res->response;
   http_response->status = endpoint->response.status;
   http_response->content_type = endpoint->response.content_type;
   http_response->endpoint_type = endpoint->response.type;
   int res = 0;
   switch (endpoint->response.type)
   {
   case ET_FILE:
      res = http_response_build_type_et_file(http_response, endpoint);
      break;

   case ET_TEXT:
      http_response_build_type_et_text(http_response, endpoint, response_content_buffer);
      break;

   case ET_FUNC:
      http_response_build_type_et_func(http_req_res, endpoint, response_content_buffer);
      break;

   case ET_DIRECTORY:
      res = http_response_build_type_et_directory(http_response, http_request, endpoint);
      break;

   case ET_PATH:
      res = -1;
   }

   if (res == -1)
   {
      http_response_build_by_type(http_req_res, error_endpoint, response_content_buffer, http_request);
   }
}

void construct_response(int client_socket, http_req_res_t *http_req_res)
{
   http_request_t *http_request = http_req_res->request;
   http_response_t *http_response = http_req_res->response;
   endpoint_t *endpoint = get_endpoint(http_tree, http_request->path);
   if (endpoint == NULL)
   {
      endpoint = error_endpoint;
   }
   char response_content_buffer[MAX_RESPONSE_SIZE];
   memset(response_content_buffer, 0, MAX_RESPONSE_SIZE);
   http_response_build_by_type(http_req_res, endpoint, response_content_buffer, http_request);

   char response_buffer[MAX_RESPONSE_SIZE] = "HTTP/1.1 ";

   char status_code[6];
   sprintf(status_code, "%d\r\n", http_response->status);
   strcat(response_buffer, status_code);

   if (http_response->content_length > 0)
   {
      char *content_type = get_content_type(http_response->content_type);
      strcat(response_buffer, "Content-Type: ");
      strcat(response_buffer, content_type);
      strcat(response_buffer, "\r\n");

      char content_length_buffer[11];
      sprintf(content_length_buffer, "%d", http_response->content_length);
      strcat(response_buffer, "Content-Length: ");
      strcat(response_buffer, content_length_buffer);
      strcat(response_buffer, "\r\n");
   }

   strcat(response_buffer, "\r\n");

   printf("-----------------\n%s\n-----------------\n", response_buffer);

   int written_size;
   int response_length = strlen(response_buffer);

#if (RESPONSE_DELAY > 0)
   usleep(RESPONSE_DELAY * 1000);
#endif

   switch (http_response->endpoint_type)
   {
   case ET_FILE:
      http_response_send_et_file(http_response, response_buffer, client_socket);
      break;

   case ET_DIRECTORY:
      http_response_send_et_file(http_response, response_buffer, client_socket);
      free(http_response->resource.file_path);
      break;

   case ET_TEXT:
      strcat(response_buffer, response_content_buffer);
      response_length += http_response->content_length;
      written_size = WRITE(SOCK, response_buffer, response_length);
      if (written_size != response_length)
      {
         printf("Error sending file\n");
      }
      break;

   case ET_FUNC:
      strcat(response_buffer, response_content_buffer);
      response_length += http_response->content_length;
      written_size = WRITE(SOCK, response_buffer, response_length);
      if (written_size != response_length)
      {
         printf("Error sending content, written size %d, expected %d\n", written_size, response_length);
      }
      else
      {
         printf("Success sending content\n");
      }
      if (http_response->content_length > 0) {
         free(http_response->resource.content);
      }
      break;

   default:
      break;
   }
}

void free_http_request(http_request_t *http_request)
{
   if (http_request->content_length > 0)
   {
      free(http_request->body);
   }
   ll_node_t *header_node;
   while ((header_node = http_request->headers) != NULL)
   {
      http_request->headers = http_request->headers->next;
      header_t *header = (header_t *)header_node->element;
      free(header->name);
      free(header->value);
      free(header);
      free(header_node);
   }
   free(http_request->path);
   free(http_request->method);
   free(http_request->http_version);
   free(http_request);
}

void free_http_response(http_response_t *http_response)
{
   ll_node_t *header_node;
   while ((header_node = http_response->headers) != NULL)
   {
      http_response->headers = http_response->headers->next;
      header_t *header = (header_t *)header_node->element;
      free(header->name);
      free(header->value);
      free(header);
      free(header_node);
   }
   free(http_response);
}

void free_req_res(http_req_res_t *http_req_res)
{
   free_http_request(http_req_res->request);
   free_http_response(http_req_res->response);
   free(http_req_res);
}

void *http_req_res_write_log_and_free(void *arg)
{
   http_req_res_t *http_req_res = (http_req_res_t *)arg;
   http_request_write_log(http_req_res->request);
   free_req_res(http_req_res);
   return NULL;
}

void accept_connection(void)
{
   sockaddr_in_t client_address;
   unsigned int client_address_len = sizeof(sockaddr_in_t);
   int client_socket;

   while (1)
   {
      client_socket = accept(sock, (struct sockaddr *)&client_address, &client_address_len);
      if (client_socket == -1)
      {
         perror("Error accepting connection");
         exit(EXIT_FAILURE);
      }
#ifdef USE_SSL
      ssl = SSL_new(ctx);
      if (ssl == NULL || SSL_set_fd(ssl, client_socket) == 0)
      {
         printf("SSL: Error setting the client socket.\n");
         // ERR_print_errors_fp(stderr);
         close(client_socket);
         continue; // Move to the next connection if SSL setup fails
      }

      if (SSL_accept(ssl) <= 0)
      {
         printf("SSL: Error with request or client sent http request.\n");
         // ERR_print_errors_fp(stderr); // Output SSL errors
         SSL_free(ssl);
         ssl = NULL;
         close(client_socket);
         continue;
      }
#endif

      http_request_t *http_request = malloc(sizeof(http_request_t));
      http_request->path = NULL;
      http_request->method = NULL;
      http_request->http_version = NULL;
      http_request->body = NULL;
      http_request->headers = NULL;
      http_request->content_length = 0;
      http_request->headers_length = 0;

      http_req_res_t *http_req_res = malloc(sizeof(http_req_res_t));
      http_req_res->request = http_request;

      char buffer[MAX_REQUEST_SIZE];
      int is_first_request = 1;
      int is_incorrect_request = 0;
      int header_parsed = 0;
      int done = 0;
      int size;
      while (!done && !is_incorrect_request && (size = READ(SOCK, buffer, MAX_REQUEST_SIZE)) > 0)
      {
         buffer[size] = '\0';
         char *buffer_ptr = buffer;
         char **request_ptr = &buffer_ptr;
         printf("%s\n", buffer);
         if (is_first_request)
         {
            if (http_request_parse_request_line(http_request, request_ptr) == -1)
            {
               is_incorrect_request = 1;
               break;
            }
            is_first_request = 0;
         }

         int header_parsing_status = http_request_parse_headers(http_request, request_ptr);
         if (!header_parsed && header_parsing_status == 1)
         {
            header_parsed = 1;
         }
         else if (header_parsing_status == -1)
         {
            is_incorrect_request = 1;
            break;
         }

         if (http_request->content_length > 0)
         {
            done = http_request_parse_body(http_request, request_ptr);
         }
         else
         {
            done = 1;
         }
         memset(buffer, 0, size);
      }
      if (!(size == -1 || is_incorrect_request))
      {
         http_response_t *http_response = malloc(sizeof(http_response_t));
         http_response->headers = NULL;
         http_req_res->response = http_response;
         construct_response(client_socket, http_req_res);
      }
      else
      {
         printf("Error with the request\n");
      }
#if (LOGGING == 1)
      pthread_t log_thread;
      pthread_create(&log_thread, NULL, http_req_res_write_log_and_free, (void *)http_req_res);
      pthread_detach(log_thread);
#else
      free_req_res(http_req_res);
#endif
#ifdef USE_SSL
      SSL_shutdown(ssl);
      SSL_free(ssl);
      ssl = NULL;
#endif
      close(client_socket);
   }
}

void start_server(int port)
{
   signal(SIGINT, sigint_handler);
   signal(SIGPIPE, SIG_IGN);
#ifdef USE_SSL
   initialize_ssl();
   ctx = create_ssl_context();
#endif

   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("Error creating socket");
      exit(EXIT_FAILURE);
   }

   int opt = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
   {
      perror("Error setting socket options");
      close(sock);
      exit(EXIT_FAILURE);
   }

   struct sockaddr_in sin;
   sin.sin_family = AF_INET;
   sin.sin_port = htons(port);
   sin.sin_addr.s_addr = INADDR_ANY;

   if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0)
   {
      perror("Error binding socket");
      close(sock);
      exit(EXIT_FAILURE);
   }

   if (listen(sock, nb_processes) != 0)
   {
      perror("Error listening on socket");
      exit(EXIT_FAILURE);
   }

   parent_pid = getpid();
   for (int i = 0; i < nb_processes; i++)
   {
      pid_t pid = fork();
      if (pid == 0)
      {
         printf("Worker process %d listening for requests\n", getpid());
         accept_connection();
         exit(0);
      }
      else if (pid < 0)
      {
         perror("Error forking process");
         exit(EXIT_FAILURE);
      }
   }

   while (wait(NULL) > 0)
      ;
#ifdef USE_SSL
   SSL_CTX_free(ctx);
#endif
   close(sock);
}
