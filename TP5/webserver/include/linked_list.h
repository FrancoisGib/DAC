#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct ll_node_s
{
   void *element;
   struct ll_node_s *next;
} ll_node_t;

typedef void (*ll_map_function_t)(ll_node_t *);

ll_node_t *insert_in_head(ll_node_t *root, void *element);
ll_node_t *insert_in_tail(ll_node_t *root, void *element);
void map_linked_list(ll_node_t *node, ll_map_function_t map_function);
void free_linked_list(ll_node_t *root, ll_map_function_t free_function);
void free_with_arg(ll_node_t *node);

#endif // LINKED_LIST_H