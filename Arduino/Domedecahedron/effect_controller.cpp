#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"


void * effect_controller_initialize(void);
void effect_controller_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    eu_palette3_t const * waffle_pal;
    eu_palette3_t const * bar_pal;
    fix16_t * lookup[DDH_TOTAL_VERTICES];
    eu_sparkle_state_t sparkle;
} effect_controller_state_t;

effect_t const effect_controller = {
    &effect_controller_initialize,
    &free,
    &effect_controller_process,
    NULL,
    NULL,
};

void effect_controller_process_sparkle(effect_controller_state_t * state,
    fix16_t delta_time, color_t buf[DDH_TOTAL_VERTICES]);

void * effect_controller_initialize(void)
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)malloc(sizeof (effect_controller_state_t));
    
    memset(state, 0, sizeof (effect_controller_state_t));
    
    state->waffle_pal = &eu_palette3_gold;
    if(eu_random() % 2 == 0) {
        state->bar_pal = &eu_palette3_adam;
    }
    else {
        state->bar_pal = &eu_palette3_peter;
    }
    eu_sparkle_initialize(&state->sparkle, fix16_from_float(1.0f));
    
    return state;
}

void effect_controller_process(void * void_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)void_state;
    color_t * temp = eu_alloc_temp_buffer();
    vector3_t slab_vec;
    
    state->time += delta_time;
    state->time %= fix16_from_int(300);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 0, 0);
    }
    
    /*
    if((fix16_t)(eu_random() % fix16_from_float(0.4f)) < delta_time) {
        size_t pos = eu_random() % DDH_TOTAL_VERTICES;
        eu_sparkle_add_pos(&state->sparkle, pos);
    }
    */
    
    
    memset(buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    eu_bar(temp, color_blend(state->bar_pal->colors[0], color_black, 192),
        vector3_make(fix16_one, 0, 0),
        di_translation.x * 3, fix16_from_float(1.0f), fix16_from_float(0.5f));
    eu_add_buffer(buf, 255, temp, 255);
    eu_bar(temp, color_blend(state->bar_pal->colors[1], color_black, 192),
        vector3_make(0, fix16_one, 0),
        di_translation.y * 3, fix16_from_float(1.0f), fix16_from_float(0.5f));
    eu_add_buffer(buf, 255, temp, 255);
    
    effect_controller_process_sparkle(state, delta_time, buf);
    
    slab_vec = vector3_make(fix16_cos(di_flat_rotation),
        fix16_sin(di_flat_rotation), 0);
    eu_bar(temp, color_blend(state->bar_pal->colors[2], color_black, 192), slab_vec,
        fix16_from_float(-0.5f), fix16_from_float(1.0f),
        fix16_from_float(0.2f));
    eu_add_buffer(buf, 255, temp, 255);
    
    eu_sparkle_process(&state->sparkle, buf, delta_time);
    
    eu_free_temp_buffer(temp);
}

void effect_controller_process_sparkle(effect_controller_state_t * state,
    fix16_t delta_time, color_t buf[DDH_TOTAL_VERTICES])
{
    if(abs(di_flat_rotation_v) > fix16_from_float(0.10f)) {
        vector3_t slab_vec = vector3_make(fix16_cos(di_flat_rotation),
            fix16_sin(di_flat_rotation), 0);
        fix16_t te = fix16_mul(delta_time, abs(di_flat_rotation_v));
        fix16_t half_width = fix16_from_float(0.5f);
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            fix16_t p = vector3_dot(slab_vec, ddh_vertex_coords_fix[i]);
            if((abs(p) < half_width) &&
                    (fix16_t)(eu_random() % fix16_from_float(5.0f)) < te) {
                eu_sparkle_add_pos(&state->sparkle, i);
            }
        }
    }
}