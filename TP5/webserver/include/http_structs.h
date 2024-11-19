#ifndef HTTP_STRUCTS_H
#define HTTP_STRUCTS_H

#include "content_type.h"
#include "http_status.h"
#include "linked_list.h"

typedef union
{
   char *file_path;
   char *content;
} response_resource_u;

typedef enum
{
   ET_FILE,
   ET_DIRECTORY,
   ET_TEXT,
   ET_PATH,
   ET_FUNC
} endpoint_type_e;

typedef struct http_response_s
{
   response_resource_u resource;
   ll_node_t *headers;
   int content_length;
   content_type_e content_type;
   http_status_e status;
   endpoint_type_e endpoint_type;
} http_response_t;

typedef struct http_request_s
{
   char *method;
   char *path;
   char *http_version;
   ll_node_t *headers;
   int headers_length;
   char *body;
   int content_length;
} http_request_t;

typedef struct
{
   http_request_t *request;
   http_response_t *response;
} http_req_res_t;

typedef void (*resource_function)(http_req_res_t *);

typedef union
{
   resource_function function;
   char *content;
} resource_t;

typedef struct
{
   resource_t resource;
   endpoint_type_e type;
   content_type_e content_type;
   http_status_e status;
} response_t;

typedef struct
{
   const char *path;
   response_t response;
} endpoint_t;

typedef struct
{
   char *name;
   char *value;
} header_t;

#endif