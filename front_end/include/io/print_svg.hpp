#ifndef PRINT_SVG_HPP
#define PRINT_SVG_HPP

#include "core/tree.hpp"

enum GraphError
{
    GRAPH_ERROR_OK        = 0,
    GRAPH_ERROR_NODE_TYPE = 1
};

GraphError  graphviz           (const Node* node, const Darray* vars);
void        print_start        (FILE* file);
GraphError  print_connections  (const Node* node, const Darray* vars, FILE* file);
void        print_end          (FILE* file);
GraphError  draw_right         (const Node* node, const Darray* vars, FILE* file);
GraphError  draw_left          (const Node* node, const Darray* vars, FILE* file);
void        graph_set_log_file (FILE* file);
void        graph_print_error  (GraphError error);
const char* graph_get_error    (GraphError error);


#endif // PRINT_SVG_HPP
