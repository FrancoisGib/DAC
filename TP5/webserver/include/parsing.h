#ifndef PARSING_H
#define PARSING_H

#include "http_structs.h"
#include "lib.h"
#include "http_tree.h"

int http_request_parse_request_line(http_request_t *http_request, char **request_ptr);
int http_request_parse_body(http_request_t *http_request, char **request_ptr);
int http_request_parse_headers(http_request_t *http_request, char **request_ptr);

#endif