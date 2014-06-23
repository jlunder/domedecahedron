#ifndef EFFECT_UTIL_H_INCLUDED
#define EFFECT_UTIL_H_INCLUDED

#include "domedecahedron.h"


extern void eu_add_buffer(color_t * dest, uint8_t dest_alpha,
    color_t * source, int32_t source_alpha);
extern void eu_add_color(color_t * dest, uint8_t dest_alpha, color_t color);
//extern void eu_modulate_contrast(color_t * dest, fix16_t amount);
extern void eu_curl(color_t * dest, color_t const * source, size_t rotation);
extern void eu_bar(color_t * dest, color_t color, vector3_t plane_origin,
    vector3_t plane_normal, fix16_t bar_size, fix16_t transition_size);

extern int32_t eu_random(void);

// This is a basic component of plasma -- sin of distance from a point
extern fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq,
    fix16_t phase);


#endif
