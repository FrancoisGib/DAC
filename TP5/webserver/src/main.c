#include "http.h"
#include "db.h"

extern int sock;
extern int nb_processes;
extern tree_t *http_tree;
extern endpoint_t *error_endpoint;

void hostname_function(http_req_res_t *http_req_res)
{
   char *str = NULL;
   int length = 0;

   if (http_req_res->request->body == NULL)
   {
      length = strlen("<p>UNAUTHORIZED</p>") + 1;
      str = malloc(length);
      strcpy(str, "<p>UNAUTHORIZED</p>");
      http_req_res->response->status = HTTP_STATUS_UNAUTHORIZED;
      http_req_res->response->content_type = HTML;
      http_req_res->response->resource.content = str;
      http_req_res->response->content_length = length;
      return;
   }

   char *username;
   char *password;
   char *body_copy = mstrdup(http_req_res->request->body);
   char *body_copy_ptr = body_copy;
   strtok_r(body_copy, ": ", &username);
   strtok_r(username, ": ", &password);
   strtok_r(password, ": ", &password);
   username = strtok_r(body_copy, ",", &body_copy);

   if (username == NULL || password == NULL)
   {
      http_req_res->response->status = HTTP_STATUS_BAD_REQUEST;
      http_req_res->response->content_length = 0;
      free(body_copy_ptr);
      return;
   }

   if (make_read_request(username, password) == 1)
   {
      char hostname[_SC_HOST_NAME_MAX + 1];
      gethostname(hostname, _SC_HOST_NAME_MAX + 1);
      char *hostname_ptr = hostname;
      length = strlen(hostname_ptr) + strlen("<p>Hostname: </p>");
      str = malloc(length + 1);
      sprintf(str, "<p>Hostname: %s</p>", hostname_ptr);
      http_req_res->response->status = HTTP_STATUS_OK;
   }
   else
   {
      length = strlen("<p>UNAUTHORIZED</p>") + 1;
      str = malloc(length);
      strcpy(str, "<p>UNAUTHORIZED</p>");
      http_req_res->response->status = HTTP_STATUS_UNAUTHORIZED;
   }

   free(body_copy_ptr);

   http_req_res->response->content_type = HTML;
   http_req_res->response->resource.content = str;
   http_req_res->response->content_length = length;
}

int main(int argc, char **argv)
{
   int port;
   if (argc > 1)
   {
      port = atoi(argv[1]);
   }
   else
   {
      printf("Please enter a port number");
      return 0;
   }
   if (argc > 2)
   {
      nb_processes = atoi(argv[2]);
   }

   const endpoint_t endpoints[] = {
       {"/", {{.content = "public/index.html"}, ET_FILE, HTML, HTTP_STATUS_OK}},
       {"/index.js", {{.content = "public/index.js"}, ET_FILE, JAVASCRIPT, HTTP_STATUS_OK}},
       {"/hostname", {{.function = hostname_function}, ET_FUNC, HTML, HTTP_STATUS_OK}},
   };

   endpoint_t error = (endpoint_t){"", {{.content = "Error"}, ET_TEXT, TEXT, HTTP_STATUS_NOT_FOUND}};
   error_endpoint = &error;

   http_tree = build_http_tree(endpoints, sizeof(endpoints) / sizeof(endpoint_t));
   print_http_tree(http_tree, 0);
   printf("Starting server on port %d\n", port);
   start_server(port);
   return 0;
}