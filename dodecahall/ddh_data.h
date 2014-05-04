#ifndef DDH_DATA_H_INCLUDED
#define DDH_DATA_H_INCLUDED


#include "dodecahall.h"


typedef struct {
    float x, y, z;
} vertex_t;

typedef uint16_t index_t;

typedef struct {
    index_t * index_start;
    size_t num_indices;
} shape_t;

typedef struct {
    shape_t * shape_start;
    size_t num_shapes;
} group_t;

extern vertex_t vertices[573];
extern uint16_t indices[720];
extern shape_t shapes[36];
extern group_t groups[8];

#endif