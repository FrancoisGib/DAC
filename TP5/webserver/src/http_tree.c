#include "http_tree.h"

const char *get_endpoint_type(endpoint_type_e type)
{
   switch (type)
   {
   case ET_FILE:
      return "FILE";
   case ET_DIRECTORY:
      return "DIRECTORY";
   case ET_PATH:
      return "PATH";
   case ET_TEXT:
      return "TEXT";
   case ET_FUNC:
      return "FUNC";
   default:
      return "";
   }
}

content_type_e get_content_type_with_file_extension(char *path)
{
   content_type_e content_type = NULL_CONTENT;
   char *extension = search_last_occurence(path, '.');
   if (extension != NULL)
   {
      extension++;
   }
   if (extension != NULL)
   {
      if (strcmp(extension, "js") == 0)
      {
         content_type = JAVASCRIPT;
      }
      else if (strcmp(extension, "json") == 0)
      {
         content_type = JSON;
      }
      else if (strcmp(extension, "html") == 0)
      {
         content_type = HTML;
      }
      else if (strcmp(extension, "txt") == 0)
      {
         content_type = TEXT;
      }
      else if (strcmp(extension, "png") == 0)
      {
         content_type = IMAGE_PNG;
      }
      else if (strcmp(extension, "ico") == 0)
      {
         content_type = IMAGE_X_ICON;
      }
      else if (strcmp(extension, "svg") == 0)
      {
         content_type = IMAGE_SVG_XML;
      }
      else if (strcmp(extension, "xml") == 0)
      {
         content_type = IMAGE_SVG_XML;
      }
   }
   return content_type;
}

const char *get_content_type(content_type_e content_type)
{
   switch (content_type)
   {
   case JSON:
      return "application/json";
   case JAVASCRIPT:
      return "text/javascript";
   case HTML:
      return "text/html";
   case TEXT:
      return "text/plain";
   case IMAGE_PNG:
      return "image/png";
   case IMAGE_X_ICON:
      return "image/x-icon";
   case IMAGE_SVG_XML:
      return "image/svg+xml";
   case NULL_CONTENT:
      return "";
   default:
      return "";
   }
}

endpoint_t *get_endpoint(tree_t *tree, char *path)
{
   if (path == NULL)
   {
      return NULL;
   }
   if (*path == '/') /* delete the / at the beginning */
   {
      path++;
   }
   endpoint_t *tree_endpoint = (endpoint_t *)tree->element;
   if (strlen(path) == 0 || strcmp("/", path) == 0)
   {
      return tree_endpoint;
   }

   char *rest = path;
   char *first_part = strtok_r(rest, "/", &rest);
   char *path_variables = NULL; // The path variable part will be improved in the future
   strtok_r(first_part, "?", &path_variables);
   if (*first_part == '?' && *path_variables == '\0')
   {
      *first_part = '\0';
   }
   if (strncmp(first_part, tree_endpoint->path, strlen(tree_endpoint->path)) == 0 && rest != NULL && *rest == '\0') // strlen(tree_endpoint->path) because if path is / then it will always be false
   {
      ll_node_t *children = tree->children;
      while (children != NULL)
      {
         tree_t *child = children->element;
         endpoint_t *child_endpoint = get_endpoint(child, first_part);
         if (child_endpoint != NULL)
         {
            return child_endpoint;
         }
         children = children->next;
      }
      if (tree_endpoint->response.type == ET_DIRECTORY || strcmp(first_part, tree_endpoint->path) == 0)
      {
         return tree_endpoint;
      }
   }

   ll_node_t *children = tree->children;
   while (children != NULL)
   {
      tree_t *child = children->element;
      endpoint_t *child_endpoint = (endpoint_t *)child->element;

      if (strncmp(child_endpoint->path, path, strlen(child_endpoint->path)) == 0 && child_endpoint->response.type == ET_DIRECTORY)
      {
         return child_endpoint;
      }
      if (strcmp(child_endpoint->path, first_part) == 0)
      {
         if (rest == NULL)
         {
            return child_endpoint;
         }
         return get_endpoint(child, rest);
      }
      children = children->next;
   }
   return NULL;
}

void add_endpoint(tree_t *tree, const char *path, response_t response)
{
   endpoint_t *tree_endpoint = (endpoint_t *)tree->element;
   if (*path == '/') /* delete the / at the beginning */
   {
      add_endpoint(tree, path + 1, response);
      return;
   }
   if ((strlen(path) == 0 || strcmp("/", path) == 0) && (strcmp("/", tree_endpoint->path) == 0 || strcmp("", tree_endpoint->path) == 0)) // for initialization (if overriding / path)
   {
      tree_endpoint->response = response;
      return;
   }
   char *rest = path;
   char *first_part = strtok_r(rest, "/", &rest);
   if (first_part == NULL || (strcmp(first_part, tree_endpoint->path) == 0 && rest == NULL))
   {
      tree_endpoint->response = response;
   }

   ll_node_t *children = tree->children;
   tree_t *child;
   int found = 0;
   while (children != NULL && !found)
   {
      child = children->element;
      endpoint_t *child_endpoint = (endpoint_t *)child->element;
      if (strcmp(child_endpoint->path, first_part) == 0)
      {
         found = 1;
      }
      else
      {
         children = children->next;
      }
   }

   if (first_part != NULL)
   {
      if (found)
      {
         if (rest != NULL)
         {
            add_endpoint(child, rest, response);
         }
      }
      else
      {
         endpoint_t *endpoint = malloc(sizeof(endpoint_t));
         endpoint->path = first_part;
         child = add_child(tree, (void *)endpoint);
         if (rest != NULL)
         {
            endpoint->response = (response_t){.type = ET_PATH, .resource.content = "", .status = HTTP_STATUS_NOT_FOUND, .content_type = NULL_CONTENT};
            add_endpoint(child, rest, response);
         }
         else
         {
            endpoint->response = response;
         }
      }
   }
}

void print_http_tree(tree_t *tree, int depth)
{
   endpoint_t *tree_endpoint = (endpoint_t *)tree->element;
   for (int i = 0; i < depth * 2; i++)
   {
      printf(" ");
   }
   printf("/%s %s %s %s\n", tree_endpoint->path, get_endpoint_type(tree_endpoint->response.type), get_content_type(tree_endpoint->response.content_type), tree_endpoint->response.type == ET_FUNC ? "" : tree_endpoint->response.resource.content);
   ll_node_t *children = tree->children;
   while (children != NULL)
   {
      print_http_tree((tree_t *)children->element, depth + 1);
      children = children->next;
   }
}

void free_http_tree(tree_t *tree)
{
   if (tree != NULL)
   {
      ll_node_t *child_node = tree->children;
      while (child_node != NULL)
      {
         ll_node_t *next_child = child_node->next;
         tree_t *child_tree = (tree_t *)child_node->element;
         free_http_tree(child_tree);
         free(child_node);
         child_node = next_child;
      }
      endpoint_t *endpoint = (endpoint_t *)tree->element;
      free(endpoint);
      free(tree);
   }
}

tree_t *init_http_tree(void)
{
   endpoint_t *endpoint = malloc(sizeof(endpoint_t));
   endpoint->path = "";
   endpoint->response = (response_t){.resource.content = "", .type = ET_PATH, .status = HTTP_STATUS_NOT_FOUND, .content_type = NULL_CONTENT};
   return init_tree((void *)endpoint);
}

tree_t *build_http_tree(const endpoint_t endpoints[], int n)
{
   tree_t *tree = init_http_tree();
   for (int i = 0; i < n; i++)
   {
      endpoint_t endpoint = endpoints[i];
      add_endpoint(tree, endpoint.path, endpoint.response);
   }
   return tree;
}