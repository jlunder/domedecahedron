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
    struct {
        fix16_t age;
        vector3_t origin;
    } shells[4];
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
void effect_controller_process_shells(effect_controller_state_t * state,
    fix16_t delta_time, color_t buf[DDH_TOTAL_VERTICES]);

void * effect_controller_initialize(void)
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)malloc(sizeof (effect_controller_state_t));
    
    memset(state, 0, sizeof (effect_controller_state_t));
    
    switch(eu_random() % 3) {
    case 0:
        state->waffle_pal = &eu_palette3_gold;
        state->bar_pal = &eu_palette3_adam;
        break;
    case 1:
        state->waffle_pal = &eu_palette3_gold;
        state->bar_pal = &eu_palette3_peter;
        break;
    case 2:
        state->waffle_pal = &eu_palette3_joe;
        state->bar_pal = &eu_palette3_primaries;
        break;
    }
    
    eu_sparkle_initialize(&state->sparkle, fix16_from_float(0.2f));
    
    for(size_t i = 0; i < LENGTHOF(state->shells); ++i) {
        state->shells[i].age = fix16_one;
    }
    
    return state;
}

void effect_controller_process(void * void_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)void_state;
    
    state->time += delta_time;
    state->time %= fix16_from_int(10);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 0, 0);
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t t = fix16_mul(state->time, fix16_two_pi);
        fix16_t x = fix16_mul(ddh_vertex_coords_fix[i].x + di_translation.x * 4, fix16_two_pi / 4);
        fix16_t y = fix16_mul(ddh_vertex_coords_fix[i].y + di_translation.y * 4, fix16_two_pi / 4);
        fix16_t s = (fix16_sin(t) / 2 + fix16_sin(x) + fix16_sin(y)) / 4 - fix16_from_int(1);
        fix16_t d = s - ddh_vertex_coords_fix[i].z;
        
        if(d > 0) {
            if(d > fix16_one) {
                buf[i] = state->waffle_pal->colors[2];
            }
            else {
                buf[i] = eu_lookup_palette3(fix16_to_int(d * 255),
                    state->waffle_pal);
            }
        }
        else {
            buf[i] = state->waffle_pal->colors[0];
        }
    }
    //memset(buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
    effect_controller_process_sparkle(state, delta_time, buf);
    effect_controller_process_shells(state, delta_time, buf);
}

void effect_controller_process_sparkle(effect_controller_state_t * state,
    fix16_t delta_time, color_t buf[DDH_TOTAL_VERTICES])
{
    color_t * temp = eu_alloc_temp_buffer();
    vector3_t slab_vec = vector3_make(fix16_cos(di_flat_rotation),
        fix16_sin(di_flat_rotation), 0);
    fix16_t t = state->time / 5;
    
    if(abs(di_flat_rotation_v) > fix16_from_float(0.10f)) {
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
    
    eu_bar(temp,
        color_blend(
            eu_lookup_palette3(
                fix16_to_int((t < fix16_one ? t : fix16_from_int(2) - t) *
                    255),
                state->bar_pal),
            color_black, 128),
        slab_vec,
        fix16_from_float(-0.5f), fix16_from_float(1.0f),
        fix16_from_float(0.2f));
    eu_add_buffer(buf, 255, temp, 255);
    
    eu_sparkle_process(&state->sparkle, buf, delta_time);
    
    eu_free_temp_buffer(temp);
}

void effect_controller_process_shells(effect_controller_state_t * state,
    fix16_t delta_time, color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t v = fix16_sqrt(vector3_lengthsq(di_translation_v));
    fix16_t thresh = fix16_from_float(1.0f);
    UNUSED(buf);
    
    if(v > fix16_from_float(0.10 * 0.10)) {
        fix16_t te = fix16_mul(delta_time, v);
        if((fix16_t)(eu_random() % fix16_from_float(2.0f)) < te) {
            for(size_t i = 0; i < LENGTHOF(state->shells); ++i) {
                if(state->shells[i].age >= fix16_one) {
                    state->shells[i].age = 0;
                    state->shells[i].origin = vector3_make(
                        eu_random() % fix16_from_int(4) - fix16_from_int(2),
                        eu_random() % fix16_from_int(4) - fix16_from_int(2),
                        eu_random() % fix16_from_int(4) - fix16_from_int(2));
                    break;
                }
            }
        }
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        vector3_t vert = ddh_vertex_coords_fix[i];
        
        for(size_t j = 0; j < LENGTHOF(state->shells); ++j) {
            if(state->shells[j].age < fix16_one) {
                fix16_t d = vector3_distsq(state->shells[j].origin, vert);
                fix16_t sd = abs(fix16_sq(state->shells[j].age * 6) - d);
                if(sd < thresh) {
                    buf[i] = color_blend(buf[i],
                        eu_lookup_palette3(
                            fix16_to_int(state->shells[j].age * 255),
                            state->bar_pal),
                        (thresh - sd) * 255 / thresh);
                }
            }
        }
    }
    
    for(size_t i = 0; i < LENGTHOF(state->shells); ++i) {
        if(state->shells[i].age < fix16_one) {
            state->shells[i].age += delta_time / 4;
        }
    }
}