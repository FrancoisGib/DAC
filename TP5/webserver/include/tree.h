#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

typedef struct
{
   void *element;
   ll_node_t *children;
} tree_t;

typedef void (*tree_map_function_t)(tree_t *);

tree_t *init_tree(void *element);
tree_t *add_child(tree_t *tree, void *element);
void free_when_elem_not_allocated(tree_t *tree);
void free_when_elem_allocated(tree_t *tree);
void map_tree(tree_t *root, tree_map_function_t map_function);
void free_tree(tree_t *tree, tree_map_function_t free_function);

#endif // TREE_H