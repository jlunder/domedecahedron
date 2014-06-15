#ifndef EFFECT_UTIL_H_INCLUDED
#define EFFECT_UTIL_H_INCLUDED

#include "domedecahedron.h"


void eu_add_buffer(color_t * dest, uint8_t dest_alpha, color_t * source,
    int32_t source_alpha);
void eu_add_color(color_t * dest, uint8_t dest_alpha, color_t color);
void eu_curl(color_t * dest, size_t rotation);

// This is a basic component of plasma -- sin of distance from a point
fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq, fix16_t phase);


#endif
