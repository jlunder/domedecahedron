#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"


void * effect_controller_initialize(void);
void effect_controller_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
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

void * effect_controller_initialize(void)
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)malloc(sizeof (effect_controller_state_t));
    
    memset(state, 0, sizeof (effect_controller_state_t));
    
    eu_sparkle_initialize(&state->sparkle, fix16_from_float(1.0f));
    
    return state;
}

void effect_controller_process(void * void_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)void_state;
    //size_t running_count = 0;
    //color_t * sum_buf = eu_alloc_temp_buffer();
    
    state->time += delta_time;
    state->time %= fix16_from_int(100);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 0, 0);
    }
    
    if((fix16_t)(eu_random() % fix16_from_float(0.4f)) < delta_time) {
        size_t pos = eu_random() % DDH_TOTAL_VERTICES;
        ddh_log("adding pos %d\n", pos);
        eu_sparkle_add_pos(&state->sparkle, pos);
    }
    
    eu_sparkle_process(&state->sparkle, buf, delta_time);
}


