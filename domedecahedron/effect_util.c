#include "effect_util.h"


eu_palette3_t eu_palette3_dusk = {{
    {{255, 127, 0, 0}},
    {{63, 0, 127, 0}},
    {{0, 0, 0, 0}},
}}; // orange -> magenta -> indigo -> black

eu_palette3_t eu_palette3_adam = {{
    {{255, 0, 255, 0}},
    {{95, 0, 127, 0}},
    {{0, 255, 255, 0}},
}}; // cyan -> magenta -> purple

eu_palette3_t eu_palette3_peter = {{
    {{0, 255, 0, 0}},
    {{63, 0, 127, 0}},
    {{0, 0, 127, 0}},
}}; // green -> indigo -> dark blue

eu_palette3_t eu_palette3_joe = {{
    {{255, 0, 0, 0}},
    {{255, 255, 0, 0}},
    {{127, 0, 127, 0}},
}}; // red -> yellow -> magenta

eu_palette3_t eu_palette3_primaries = {{
    {{255, 0, 0, 0}},
    {{0, 255, 0, 0}},
    {{0, 0, 255, 0}},
}}; // red -> green -> blue

eu_palette3_t eu_palette3_gold = {{
    {{0, 0, 0, 0}},
    {{255, 255, 0, 0}},
    {{255, 255, 255, 0}},
}}; // red -> green -> blue

uint32_t eu_random_z = 392842;
uint32_t eu_random_w = 43503;

color_t eu_temp_buffers[EU_MAX_TEMP_BUFFERS][DDH_TOTAL_VERTICES];
color_t * eu_free_temp_buffers[EU_MAX_TEMP_BUFFERS] = {
    eu_temp_buffers[0],
    eu_temp_buffers[1],
    eu_temp_buffers[2],
    eu_temp_buffers[3],
    eu_temp_buffers[4],
    eu_temp_buffers[5],
    eu_temp_buffers[6],
    eu_temp_buffers[7],
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

void eu_temporal_iir_one_pole(color_t dest_accum[DDH_TOTAL_VERTICES],
    color_t source[DDH_TOTAL_VERTICES], fix16_t k)
{
    fix16_t one_minus_k = fix16_one - k;
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t r = (fix16_t)dest_accum[i].r * one_minus_k +
            (fix16_t)source[i].r * k;
        fix16_t g = (fix16_t)dest_accum[i].g * one_minus_k +
            (fix16_t)source[i].g * k;
        fix16_t b = (fix16_t)dest_accum[i].b * one_minus_k +
            (fix16_t)source[i].b * k;
        
        dest_accum[i] =
            color_make(fix16_to_int(r), fix16_to_int(g), fix16_to_int(b));
    }
}

void eu_color_seq_0(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    // fill one light at a time from 0 -> n-1 with color 0, then color 1, 2...
    size_t sweep_bg_color_index = (time / fix16_one) % num_seq_colors;
    size_t sweep_fg_color_index = ((time / fix16_one) + 1) % num_seq_colors;
    size_t sweep_dest_index = fix16_to_int((time % fix16_one) *
        num_dest_colors);
    color_t bg_color = seq_colors[sweep_bg_color_index];
    color_t fg_color = seq_colors[sweep_fg_color_index];
    
    for(size_t i = 0; i < sweep_dest_index; ++i) {
        dest[i] = fg_color;
    }
    for(size_t i = sweep_dest_index; i < num_dest_colors; ++i) {
        dest[i] = bg_color;
    }
}

void eu_color_seq_1(color_t * dest, size_t num_dest_colors,
    fix16_t time, color_t const * seq_colors, size_t num_seq_colors)
{
    eu_color_seq_0(dest, num_dest_colors,
        fix16_from_int(num_dest_colors) -
            (time % fix16_from_int(num_seq_colors)),
        seq_colors, num_seq_colors);
}

void eu_color_seq_2(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    fix16_t scale = fix16_one / num_dest_colors;
    // rotate solid bars...
    for(size_t i = 0; i < num_dest_colors; ++i) {
        size_t j = fix16_to_int(fix16_mul(
                ((time * num_dest_colors +
                    fix16_from_int(i)) * num_seq_colors),
            scale)) % num_seq_colors;
        dest[i] = seq_colors[j];
    }
}

void eu_color_seq_3(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    eu_color_seq_2(dest, num_dest_colors,
        fix16_one -
            (time % fix16_one),
        seq_colors, num_seq_colors);
}

void eu_color_seq_4(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    fix16_t alpha = time % fix16_one;
    fix16_t one_minus_alpha = fix16_one - alpha;
    size_t color_index = (time / fix16_one) % num_seq_colors;
    color_t ca = seq_colors[color_index];
    color_t cb = seq_colors[(color_index + 1) % num_seq_colors];
    color_t c = color_make(
        fix16_to_int(ca.r * one_minus_alpha + cb.r * alpha),
        fix16_to_int(ca.g * one_minus_alpha + cb.g * alpha),
        fix16_to_int(ca.b * one_minus_alpha + cb.b * alpha));
    
    // fade from color 0 -> 1 -> 2...
    for(size_t i = 0; i < num_dest_colors; ++i) {
        dest[i] = c;
    }
}

void eu_color_seq_5(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    // fade from color 0 -> black -> 1 -> black -> 2...
    fix16_t t = time % fix16_one;
    fix16_t alpha = (t < (fix16_one / 2)) ? t * 2 : (fix16_one - t) * 2;
    color_t ca = seq_colors[(time / fix16_one) % num_seq_colors];
    color_t c = color_make(
        fix16_to_int(ca.r * alpha),
        fix16_to_int(ca.g * alpha),
        fix16_to_int(ca.b * alpha));
    
    // fade into/out of each color
    for(size_t i = 0; i < num_dest_colors; ++i) {
        dest[i] = c;
    }
}

void eu_color_seq_6(color_t * dest, size_t num_dest_colors, fix16_t time,
    color_t const * seq_colors, size_t num_seq_colors)
{
    // fade from color 0 -> black -> 1 -> black -> 2...
    fix16_t t = time % fix16_one;
    fix16_t alpha = (t < (fix16_one / 2)) ? t * 2 : (fix16_one - t) * 2;
    color_t ca = seq_colors[(time / fix16_one) % num_seq_colors];
    color_t c = color_make(
        fix16_to_int(ca.r * alpha),
        fix16_to_int(ca.g * alpha),
        fix16_to_int(ca.b * alpha));
    
    // fade into/out of each color
    for(size_t i = 0; i < num_dest_colors; ++i) {
        dest[i] = c;
    }
}

color_t eu_lookup_palette3(uint_fast8_t pos, eu_palette3_t const * pal)
{
    size_t index = (pos >> 7) & 0x1;
    uint32_t alpha = pos & 0x7F;
    uint32_t one_minus_alpha = 0x80 - alpha;
    
    return color_make((pal->colors[index + 1].r * alpha +
            pal->colors[index].r * one_minus_alpha) >> 7,
        (pal->colors[index + 1].g * alpha +
            pal->colors[index].g * one_minus_alpha) >> 7,
        (pal->colors[index + 1].b * alpha +
            pal->colors[index].b * one_minus_alpha) >> 7);
}

color_t eu_lookup_palette3_random(eu_palette3_t const * pal)
{
    static fix16_t const _0_25 = (fix16_t)(fix16_one * 0.25f + 0.5f);
    static fix16_t const _0_4 = (fix16_t)(fix16_one * 0.4f + 0.5f);
    static fix16_t const _0_6 = (fix16_t)(fix16_one * 0.6f + 0.5f);
    static fix16_t const _0_75 = (fix16_t)(fix16_one * 0.75f + 0.5f);
    static fix16_t const _0_25_inv_0_4 = (fix16_t)(fix16_one * 0.25f / 0.4f + 0.5f);
    static fix16_t const _0_5_inv_0_2 = (fix16_t)(fix16_one * 0.5f / 0.2f + 0.5f);
    
    fix16_t alpha = eu_random() % fix16_one;

    if(alpha < _0_4) {
        alpha = fix16_mul(_0_25_inv_0_4, alpha);
    }
    else if(alpha < _0_6) {
        alpha = fix16_mul(_0_5_inv_0_2, alpha - _0_4) + _0_25;
    }
    else {
        alpha = fix16_mul(_0_25_inv_0_4, alpha - _0_6) + _0_75;
    }
    
    if(alpha >= fix16_one) {
        alpha = fix16_one - 1;
    }
    
    return eu_lookup_palette3(fix16_to_int(alpha * 256), pal);
}

uint32_t eu_random(void)
{
    eu_random_z = 36969 * (eu_random_z & 65535) + (eu_random_z >> 16);
    eu_random_w = 18000 * (eu_random_w & 65535) + (eu_random_w >> 16);
    return (eu_random_z << 16) + eu_random_w;  /* 32-bit result */
}    

fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq, fix16_t phase)
{
    fix16_t dist = fix16_sqrt(vector3_distsq(a, b));
    fix16_t phi = (fix16_mul(dist, freq) + phase) % fix16_two_pi;
    
    if(phi < 0) {
        phi += fix16_two_pi;
    }
    
    return -fix16_sin_parabola(phi - fix16_pi);
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

