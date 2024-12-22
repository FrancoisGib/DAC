#include "http.h"
#include "db.h"
#include "linpack.h"

extern int sock;
extern int nb_processes;
extern tree_t *http_tree;
extern endpoint_t *error_endpoint;

extern void *mempool;

void linpack_function(http_req_res_t* http_req_res)
{
   char    *arsize_input;
   int     arsize;
   long    arsize2d,nreps;
   size_t  malloc_arg,memreq;

   arsize_input = getenv("LINPACK_ARRAY_SIZE");
   if (arsize_input == NULL) {
      arsize = 200;
   } else {
      arsize = atoi(arsize_input);
   }
   arsize/=2;
   arsize*=2;
   if (arsize<10)
      {
      printf("Too small.\n");
      return 1;
      }
   arsize2d = (long)arsize*(long)arsize;
   memreq=arsize2d*sizeof(REAL)+(long)arsize*sizeof(REAL)+(long)arsize*sizeof(int);
   printf("Memory required:  %ldK.\n",(memreq+512L)>>10);
   malloc_arg=(size_t)memreq;
   if (malloc_arg!=memreq || (mempool=malloc(malloc_arg))==NULL)
      {
      printf("Not enough memory available for given array size.\n\n");
      return 2;
      }
   printf("\n\nLINPACK benchmark, %s precision.\n",PREC);
   printf("Machine precision:  %d digits.\n",BASE10DIG);
   printf("Array size %d X %d.\n",arsize,arsize);
   printf("Average rolled and unrolled performance:\n\n");
   printf("    Reps Time(s) DGEFA   DGESL  OVERHEAD    KFLOPS\n");
   printf("----------------------------------------------------\n");
   nreps=1;
   while (linpack(nreps,arsize)<10.)
      nreps*=2;
   free(mempool);
   printf("\n");
   http_req_res->response->status = HTTP_STATUS_OK;
   http_req_res->response->content_type = NULL_CONTENT;
   http_req_res->response->content_length = 0;
}

#define LOOP_COUNT 4294967295

void *thread_wrapper(void* arg)
{
   unsigned int var = 4294967295; // max uint32 size
   for (unsigned int i = 0; i < LOOP_COUNT; i++) {
      var /= 1; // div takes time
      var = 4294967295;
   }
   return NULL;
}

void loop_function(http_req_res_t* http_req_res)
{
   http_req_res->response->content_length = 0;
   http_req_res->response->content_type = NULL_CONTENT;
   int nb_threads;
   if (sscanf(http_req_res->request->body, "NB_THREADS %d", &nb_threads) == 1) {
      pthread_t threads[nb_threads];
      for (int i = 0; i < nb_threads; i++) {
         pthread_create(&threads[i], NULL, thread_wrapper, NULL);
      }
      for (int i = 0; i < nb_threads; i++) {
         pthread_join(threads[i], NULL);
      }
      http_req_res->response->status = HTTP_STATUS_OK;
   }
   else {
      http_req_res->response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
   }
}

void delay_function(http_req_res_t* http_req_res)
{
   int delay;
   if (sscanf(http_req_res->request->body, "%d", &delay) == 1) {
      http_req_res->response->status = HTTP_STATUS_OK;
      sleep(delay);
   }
   else {
      http_req_res->response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
   }
   http_req_res->response->content_length = 0;
   http_req_res->response->content_type = NULL_CONTENT;
}

unsigned long long memory_testing_size = 0;
#define MEMORY_ALLOC_GAP 10000000

void* free_wrapper(void* arg) {
   free(arg);
   sleep(5);
   return NULL;
}

void use_memory_function(http_req_res_t* http_req_res) {
   if (strcmp(http_req_res->request->body, "INCREASE") == 0) {
      memory_testing_size += MEMORY_ALLOC_GAP;
   }
   else if (memory_testing_size >= MEMORY_ALLOC_GAP && strcmp(http_req_res->request->body, "DECREASE") == 0) {
      memory_testing_size -= MEMORY_ALLOC_GAP;
   }
   else {
      int new_memory_size;
      if (sscanf(http_req_res->request->body, "SET %d", &new_memory_size) == 1) {
         memory_testing_size = new_memory_size * 1000000;
      }
      else {
         memory_testing_size = 0;
         http_req_res->response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
         http_req_res->response->content_length = 0;
         return;
      }
   }

   if (memory_testing_size == 0) {
      memory_testing_size = 0;
      http_req_res->response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
      http_req_res->response->content_length = 0;
      return;
   }

   void* memory_testing_ptr = malloc(MEMORY_ALLOC_GAP);

   if (memory_testing_ptr == NULL) {
      http_req_res->response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
      http_req_res->response->content_length = 0;
   }
   else {
      char* body = malloc(32);
      sprintf(body, "%lld", memory_testing_size);
      http_req_res->response->content_length = strlen(body);
      http_req_res->response->resource.content = body;
      http_req_res->response->content_type = JSON;
      http_req_res->response->status = HTTP_STATUS_OK;
   }
   pthread_t free_thread;
   pthread_create(&free_thread, NULL, free_wrapper, memory_testing_ptr);
   pthread_detach(free_thread);
   printf("Memory: %lld\n", memory_testing_size);
}

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
       {"/memory-test", {{.function = use_memory_function}, ET_FUNC, JSON, HTTP_STATUS_OK}},
       {"/calculation-test", {{.function = loop_function}, ET_FUNC, NULL_CONTENT, HTTP_STATUS_OK}},
       {"/delay-test", {{.function = delay_function}, ET_FUNC, NULL_CONTENT, HTTP_STATUS_OK}},
       {"/linpack-test", {{.function = linpack_function}, ET_FUNC, NULL_CONTENT, HTTP_STATUS_OK}},
   };

   endpoint_t error = (endpoint_t){"", {{.content = "Error"}, ET_TEXT, TEXT, HTTP_STATUS_NOT_FOUND}};
   error_endpoint = &error;

   http_tree = build_http_tree(endpoints, sizeof(endpoints) / sizeof(endpoint_t));
   print_http_tree(http_tree, 0);
   printf("Starting server on port %d\n", port);
   start_server(port);
   return 0;
}