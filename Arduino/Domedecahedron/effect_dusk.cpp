#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"


void * effect_dusk_initialize(void);
void effect_dusk_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    fix16_t no_interaction_time;
    size_t stars_pos[50];
    vector3_t auroras_pos[3];
    color_t buf[DDH_TOTAL_VERTICES];
    color_t seq_colors[4];
} effect_dusk_state_t;

effect_t const effect_dusk = {
    &effect_dusk_initialize,
    &free,
    &effect_dusk_process,
    NULL,
    NULL,
};

void * effect_dusk_initialize(void)
{
    effect_dusk_state_t * state =
        (effect_dusk_state_t *)malloc(sizeof (effect_dusk_state_t));
    
    memset(state, 0, sizeof (*state));
    
    for(size_t i = 0; i < LENGTHOF(state->stars_pos); ++i) {
        state->stars_pos[i] = eu_random() % DDH_TOTAL_VERTICES;
    }
    
    for(size_t i = 0; i < LENGTHOF(state->auroras_pos); ++i) {
        state->auroras_pos[i].x = (eu_random() % fix16_one) * 2 - fix16_one;
        state->auroras_pos[i].y =
            (eu_random() % fix16_one) * 4 - fix16_from_int(2);
        state->auroras_pos[i].z = fix16_from_float(0.75f);
    }
    
    for(size_t i = 0; i < LENGTHOF(state->seq_colors); ++i) {
        state->seq_colors[i] = eu_lookup_palette3_random(&eu_palette3_adam);
    }
    
    return state;
}

void effect_dusk_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t in0 = fix16_one - di_treated_z[0];
    fix16_t in1 = fix16_one - di_treated_z[1];
    fix16_t in2 = fix16_one - di_treated_z[2];
    
    fix16_t auroras_v = fix16_from_float(0.75f);
    effect_dusk_state_t * state = (effect_dusk_state_t *)voidp_state;
    fix16_t t = state->time / 300;
    vector3_t up_vec = vector3_normalize(vector3_make(0,
        fix16_from_float(0.3f), fix16_one));
    fix16_t bias;
    color_t aurora_color;
    
    if(in0 == 0 && in1 == 0 && in2 == 0) {
        state->no_interaction_time += delta_time;
        if(state->no_interaction_time > fix16_from_int(10)) {
            in0 = (fix16_sin(fix16_mul(state->no_interaction_time,
                    fix16_two_pi * 1 / 26)) + fix16_one) / 2;
            in1 = (fix16_sin(fix16_mul(fix16_sin(
                fix16_mul(state->no_interaction_time,
                fix16_two_pi * 3 / 29)), fix16_pi)) + fix16_one) / 2;
            in2 = (eu_random() % fix16_from_int(1)) < delta_time ?
                eu_random() % fix16_one : 0;
        }
    }
    else {
        state->no_interaction_time = 0;
    }
    
    t = in0;
    aurora_color = eu_lookup_palette3(fix16_to_int(in1 * 255),
        &eu_palette3_peter);
    
    up_vec.x = fix16_mul(up_vec.x, fix16_from_float(0.35f));
    up_vec.y = fix16_mul(up_vec.y, fix16_from_float(0.35f));
    up_vec.z = fix16_mul(up_vec.z, fix16_from_float(0.35f));
    
    bias = fix16_from_float(0.4f) + t / 2;
    
    state->time += delta_time;
    
    for(size_t i = 0; i < LENGTHOF(state->auroras_pos); ++i) {
        state->auroras_pos[i].y -= fix16_mul(delta_time, auroras_v);
        if(state->auroras_pos[i].y < fix16_from_int(-2)) {
            state->auroras_pos[i].y += fix16_from_int(4);
        }
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        vector3_t vc = ddh_vertex_coords_fix[i];
        fix16_t pos = vector3_dot(up_vec, vc) + bias;
        int32_t pal_pos = fix16_to_int(pos * 256);
        color_t c = eu_lookup_palette3(
            pal_pos > 255 ? 255 : (pal_pos < 0 ? 0 : pal_pos),
            &eu_palette3_dusk);
        
        if(ddh_vertex_coords_fix[i].z > fix16_from_int(-1)) {
            fix16_t al = 0;
            for(size_t j = 0; j < LENGTHOF(state->auroras_pos); ++j) {
                fix16_t dsq = vector3_distsq(state->auroras_pos[j], vc);
                if(dsq < fix16_one) {
                    al += fix16_one - dsq;
                }
            }
            
            if(al > fix16_one) {
                al = fix16_one;
            }
            
            c = color_add_sat(c,
                color_make(fix16_to_int(al * aurora_color.r),
                    fix16_to_int(al * aurora_color.g),
                    fix16_to_int(al * aurora_color.b)));
        }
        buf[i] = c;
    }
    
    for(size_t i = 0; i < LENGTHOF(state->stars_pos); ++i) {
        uint_fast8_t l = fix16_to_int(t * (eu_random() % 256));
        
        if(eu_random() % fix16_from_int(30) < delta_time) {
            state->stars_pos[i] = eu_random() % DDH_TOTAL_VERTICES;
        }
        
        buf[state->stars_pos[i]] = color_add_sat(buf[state->stars_pos[i]],
            color_make(l, l, l));
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        uint32_t r = (uint32_t)state->buf[i].r * 15 + (uint32_t)buf[i].r * 1;
        uint32_t g = (uint32_t)state->buf[i].g * 15 + (uint32_t)buf[i].g * 1;
        uint32_t b = (uint32_t)state->buf[i].b * 15 + (uint32_t)buf[i].b * 1;
        state->buf[i] = buf[i] = color_make(r / 16, g / 16, b / 16);
    }
}
