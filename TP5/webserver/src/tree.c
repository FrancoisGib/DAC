#include "tree.h"

tree_t *init_tree(void *element)
{
   tree_t *tree = malloc(sizeof(tree_t));
   if (tree == NULL)
   {
      return NULL;
   }
   tree->children = NULL;
   tree->element = element;
   return tree;
}

tree_t *add_child(tree_t *tree, void *element)
{
   tree_t *child = init_tree(element);
   tree->children = insert_in_tail(tree->children, child);
   return child;
}

void free_when_elem_not_allocated(tree_t *tree)
{
   if (tree != NULL)
   {
      ll_node_t *children = tree->children;
      free_linked_list(children, (ll_map_function_t)free);
      free(tree);
   }
}

void free_when_elem_allocated(tree_t *tree)
{
   if (tree != NULL)
   {
      ll_node_t *children = tree->children;
      free_linked_list(children, (ll_map_function_t)free);
      free(tree->element);
      free(tree);
   }
}

void map_tree(tree_t *root, tree_map_function_t map_function)
{
   if (map_function == NULL || root == NULL)
   {
      return;
   }

   map_function(root);
   ll_node_t *children = root->children;
   while (children != NULL)
   {
      tree_t *child = (tree_t *)children->element;
      map_function(child);
      children = children->next;
   }
}

void free_tree(tree_t *tree, tree_map_function_t free_function)
{
   if (free_function == NULL || tree == NULL)
   {
      return;
   }

   ll_node_t *children = tree->children;
   while (children != NULL)
   {
      tree_t *child = (tree_t *)children->element;
      free_function(child);
      children = children->next;
   }
   free_function(tree);
}