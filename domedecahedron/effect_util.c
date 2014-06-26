#include "effect_util.h"


int32_t eu_last_random = 0;
color_t eu_temp_buffers[EU_MAX_TEMP_BUFFERS][DDH_TOTAL_VERTICES];
color_t * eu_free_temp_buffers[EU_MAX_TEMP_BUFFERS] = {
    eu_temp_buffers[0],
    eu_temp_buffers[1],
    eu_temp_buffers[2],
    eu_temp_buffers[3],
};
size_t eu_free_temp_buffers_top = EU_MAX_TEMP_BUFFERS;


void eu_add_buffer(color_t dest[DDH_TOTAL_VERTICES], uint8_t dest_alpha,
    color_t const source[DDH_TOTAL_VERTICES], uint8_t source_alpha)
{
    if(dest_alpha == 255 && source_alpha == 255) {
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            dest[i] = color_add_sat(dest[i], source[i]);
        }
    }
    else {
        dest_alpha += 1;
        source_alpha += 1;
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            uint32_t r = (uint32_t)dest[i].r * dest_alpha +
                (uint32_t)source[i].r * source_alpha;
            uint32_t g = (uint32_t)dest[i].g * dest_alpha +
                (uint32_t)source[i].g * source_alpha;
            uint32_t b = (uint32_t)dest[i].b * dest_alpha +
                (uint32_t)source[i].b * source_alpha;
            
            if(r > 255 * 256) r = 255 * 256;
            if(g > 255 * 256) g = 255 * 256;
            if(b > 255 * 256) b = 255 * 256;
            
            dest[i] = color_make(r / 256, g / 256, b / 256);
        }
    }
}

void eu_add_color(color_t * dest, uint8_t dest_alpha, color_t color)
{
    if(dest_alpha == 255) {
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            dest[i] = color_add_sat(dest[i], color);
        }
    }
    else {
        uint32_t color_r = color.r * 256;
        uint32_t color_g = color.g * 256;
        uint32_t color_b = color.b * 256;
        
        dest_alpha += 1;
        
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            uint32_t r = (uint32_t)dest[i].r * dest_alpha + color_r;
            uint32_t g = (uint32_t)dest[i].g * dest_alpha + color_g;
            uint32_t b = (uint32_t)dest[i].b * dest_alpha + color_b;
            
            if(r > 255 * 256) r = 255 * 256;
            if(g > 255 * 256) g = 255 * 256;
            if(b > 255 * 256) b = 255 * 256;
            
            dest[i] = color_make(r / 256, g / 256, b / 256);
        }
    }
}

void eu_scramble(color_t dest[DDH_TOTAL_VERTICES],
    color_t const source[DDH_TOTAL_VERTICES], size_t group_rotation,
    size_t dodecahedron_rotation, size_t vertex_rotation)
{
    size_t vertex_offsets[DDH_VERTICES_PER_DODECAHEDRON];
    
    for(size_t i = 0; i < DDH_VERTICES_PER_DODECAHEDRON; ++i) {
        vertex_offsets[i] =
            (i + vertex_rotation) % DDH_VERTICES_PER_DODECAHEDRON;
    }
    
    if(group_rotation == 0 && dodecahedron_rotation == 0) {
        for(size_t i = 0; i < DDH_TOTAL_GROUPS; ++i) {
            for(size_t j = 0; j < DDH_DODECAHEDRONS_PER_GROUP; ++j) {
                for(size_t k = 0; k < DDH_VERTICES_PER_DODECAHEDRON; ++k) {
                    dest[ddh_group_dodecahedron_vertex_offsets[i][j][k]] =
                        source[ddh_group_dodecahedron_vertex_offsets[i][j][
                            vertex_offsets[k]]];
                }
            }
        }
    }
    else {
        size_t group_offsets[DDH_TOTAL_GROUPS];
        size_t dodecahedron_offsets[DDH_DODECAHEDRONS_PER_GROUP];
    
        for(size_t i = 0; i < DDH_TOTAL_GROUPS; ++i) {
            group_offsets[i] =
                (i + group_rotation) % DDH_TOTAL_GROUPS;
        }
        for(size_t i = 0; i < DDH_DODECAHEDRONS_PER_GROUP; ++i) {
            dodecahedron_offsets[i] =
                (i + dodecahedron_rotation) % DDH_DODECAHEDRONS_PER_GROUP;
        }
    
        for(size_t i = 0; i < DDH_TOTAL_GROUPS; ++i) {
            for(size_t j = 0; j < DDH_DODECAHEDRONS_PER_GROUP; ++j) {
                for(size_t k = 0; k < DDH_VERTICES_PER_DODECAHEDRON; ++k) {
                    dest[ddh_group_dodecahedron_vertex_offsets[i][j][k]] =
                        source[ddh_group_dodecahedron_vertex_offsets[
                            group_offsets[i]][dodecahedron_offsets[j]][
                            vertex_offsets[k]]];
                }
            }
        }
    }
}

void eu_bar(color_t dest[DDH_TOTAL_VERTICES], color_t color,
    vector3_t plane_normal, fix16_t plane_position, fix16_t bar_size,
    fix16_t transition_size)
{
    fix16_t inv_transition_size =
        fix16_div(fix16_from_int(1), transition_size);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t pos = vector3_dot(ddh_vertex_coords_fix[i], plane_normal) -
            plane_position;
        
        if((pos < -transition_size) || (pos >= bar_size + transition_size)) {
            dest[i] = color_black;
        }
        else if(pos >= 0 && pos < bar_size) {
            dest[i] = color;
        }
        else {
            fix16_t alpha = 0;
            
            if(pos > bar_size) {
                alpha = fix16_mul(transition_size - (pos - bar_size) - 1,
                    inv_transition_size);
            }
            else if(pos < 0) {
                alpha = fix16_mul(transition_size + pos, inv_transition_size);
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

color_t * eu_alloc_temp_buffer(void)
{
    assert(eu_free_temp_buffers_top > 0);
    
    if(eu_free_temp_buffers_top > 0) {
        --eu_free_temp_buffers_top;
        return eu_free_temp_buffers[eu_free_temp_buffers_top];
    }
    else {
        return NULL;
    }
}

void eu_free_temp_buffer(color_t * buf)
{
    assert(eu_free_temp_buffers_top < EU_MAX_TEMP_BUFFERS);
    assert(buf != NULL);
    
    if(buf == NULL) {
        return;
    }
    
    eu_free_temp_buffers[eu_free_temp_buffers_top] = buf;
    ++eu_free_temp_buffers_top;
}

