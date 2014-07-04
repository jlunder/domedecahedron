#include "effect.h"

#include "effect_util.h"


void * effect_rings_0_initialize(void);
void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    color_t buf[DDH_TOTAL_VERTICES];
} effect_rings_0_state_t;

effect_t const effect_rings_0 = {
    &effect_rings_0_initialize,
    &free,
    &effect_rings_0_process,
    NULL,
    NULL,
};

void * effect_rings_0_initialize(void)
{
    effect_rings_0_state_t * rings_state =
        (effect_rings_0_state_t *)malloc(sizeof (effect_rings_0_state_t));
    
    memset(rings_state, 0, sizeof (effect_rings_0_state_t));
    
    return rings_state;
}

void effect_rings_0_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_rings_0_state_t * state = (effect_rings_0_state_t *)voidp_state;
    color_t * sum_buf = eu_alloc_temp_buffer();
    color_t * temp_buf = eu_alloc_temp_buffer();
    fix16_t ring_a_t = state->time % fix16_from_int(8) / 8;
    
    
    state->time += delta_time;
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 127, 255);
    }
    
    eu_bar(temp_buf, color_white, vector3_make(0, 0, fix16_one),
        fix16_mul(ring_a_t, fix16_from_float(3.5f)) - fix16_from_float(3.f),
        fix16_from_float(0.5f),
        fix16_from_float(0.1f));
    eu_add_buffer(sum_buf, 255, temp_buf, 255);
    
    memcpy(sum_buf, temp_buf, sizeof state->buf);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        uint32_t r = (uint32_t)state->buf[i].r * 15 + (uint32_t)sum_buf[i].r * 1;
        uint32_t g = (uint32_t)state->buf[i].g * 15 + (uint32_t)sum_buf[i].g * 1;
        uint32_t b = (uint32_t)state->buf[i].b * 15 + (uint32_t)sum_buf[i].b * 1;
        state->buf[i] = color_make(r / 16, g / 16, b / 16);
    }
    
    eu_free_temp_buffer(temp_buf);
    eu_free_temp_buffer(sum_buf);
    
    memcpy(buf, state->buf, sizeof state->buf);
}


