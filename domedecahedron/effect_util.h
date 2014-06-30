#ifndef EFFECT_UTIL_H_INCLUDED
#define EFFECT_UTIL_H_INCLUDED

#include "domedecahedron.h"


#define EU_MAX_TEMP_BUFFERS 8


typedef struct {
    color_t colors[3];
} eu_palette3_t;


extern eu_palette3_t eu_palette3_dusk; // orange -> magenta -> indigo
extern eu_palette3_t eu_palette3_adam; // cyan -> magenta -> purple
extern eu_palette3_t eu_palette3_peter; // green -> indigo -> dark blue
extern eu_palette3_t eu_palette3_joe; // red -> yellow -> magenta
extern eu_palette3_t eu_palette3_primaries; // red -> green -> blue


extern void eu_add_buffer(color_t dest[DDH_TOTAL_VERTICES],
    uint8_t dest_alpha, color_t const source[DDH_TOTAL_VERTICES],
    uint8_t source_alpha);
extern void eu_add_color(color_t dest[DDH_TOTAL_VERTICES], uint8_t dest_alpha,
    color_t color);
extern void eu_scramble(color_t dest[DDH_TOTAL_VERTICES],
    color_t const source[DDH_TOTAL_VERTICES], size_t group_rotation,
    size_t dodecahedron_rotation, size_t vertex_rotation);
extern void eu_bar(color_t dest[DDH_TOTAL_VERTICES], color_t color,
    vector3_t plane_normal, fix16_t plane_position, fix16_t bar_size,
    fix16_t transition_size);

extern color_t eu_lookup_palette3(uint_fast8_t pos, eu_palette3_t const * pal);

extern int32_t eu_random(void);

// This is a basic component of plasma -- sin of distance from a point
extern fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq,
    fix16_t phase);

extern color_t * eu_alloc_temp_buffer(void);
extern void eu_free_temp_buffer(color_t * buf);


#endif
