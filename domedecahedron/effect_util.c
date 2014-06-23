#include "effect_util.h"


int32_t eu_last_random = 0;


void eu_add_buffer(color_t * dest, uint8_t dest_alpha, color_t * source,
    int32_t source_alpha)
{
    if(dest_alpha == 255 && source_alpha == 255) {
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            dest[i] = color_add_sat(dest[i], source[i]);
        }
    }
}


void eu_add_color(color_t * dest, uint8_t dest_alpha, color_t color)
{
}


void eu_curl(color_t * dest, color_t const * source, size_t rotation)
{
    size_t offsets[DDH_VERTICES_PER_DODECAHEDRON];
    
    for(size_t i = 0; i < DDH_VERTICES_PER_DODECAHEDRON; ++i) {
        offsets[i] = (i + rotation) % DDH_VERTICES_PER_DODECAHEDRON;
    }
    
    for(size_t i = 0; i < DDH_TOTAL_GROUPS; ++i) {
        for(size_t j = 0; j < DDH_DODECAHEDRONS_PER_GROUP; ++j) {
            for(size_t k = 0; k < (DDH_VERTICES_PER_DODECAHEDRON - 1); ++k) {
                dest[ddh_group_dodecahedron_vertex_offsets[i][j][k]] =
                    source[ddh_group_dodecahedron_vertex_offsets[i][j][
                        offsets[k]]];
            }
        }
    }
}


void eu_bar(color_t * dest, color_t color, vector3_t plane_origin,
    vector3_t plane_normal, fix16_t bar_size, fix16_t transition_size)
{
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t pos = vector3_dot(vector3_sub(ddh_vertex_coords_fix[i],
            plane_origin), plane_normal);
        
        if((pos < -transition_size) || (pos >= bar_size + transition_size)) {
            dest[i] = color_black;
        }
        else if(pos >= 0 && pos < bar_size) {
            dest[i] = color;
        }
        else {
            fix16_t alpha = 0;
            
            if(pos > bar_size) {
                alpha = transition_size - (pos - bar_size) - 1;
            }
            else if(pos < 0) {
                alpha = transition_size + pos;
            }
            
            if(alpha > 0) {
                dest[i] = color_make(fix16_to_int(color.r * alpha),
                    fix16_to_int(color.g * alpha),
                    fix16_to_int(color.b * alpha));
            }
        }
    }
}


int32_t eu_random(void)
{
    eu_last_random =
        ((eu_last_random + (int32_t)ddh_total_ns) * 1999999973L) & 0x7FFFFFFF;
    
    return eu_last_random;
}


fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq, fix16_t phase)
{
    fix16_t dist = fix16_sqrt(
        fix16_sq(a.x - b.x) +
        fix16_sq(a.y - b.y) +
        fix16_sq(a.z - b.z));
    
    return fix16_sin(fix16_mul(dist, freq) + phase);
}


