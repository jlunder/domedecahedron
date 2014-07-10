#include "effect.h"

#include "effect_util.h"


void * effect_rings_2_initialize(void);
void effect_rings_2_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    struct {
        eu_palette3_t const * pal;
        vector3_t pos;
        bool running;
    } rings[4];
} effect_rings_2_state_t;

effect_t const effect_rings_2 = {
    &effect_rings_2_initialize,
    &free,
    &effect_rings_2_process,
    NULL,
    NULL,
};

void * effect_rings_2_initialize(void)
{
    effect_rings_2_state_t * rings_state =
        (effect_rings_2_state_t *)malloc(sizeof (effect_rings_2_state_t));
    
    memset(rings_state, 0, sizeof (effect_rings_2_state_t));
    
    return rings_state;
}

void effect_rings_2_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    //effect_rings_2_state_t * rings_state = (effect_rings_2_state_t *)state;
    //size_t running_count = 0;
    //color_t * sum_buf = eu_alloc_temp_buffer();
    
    UNUSED(delta_time);
}


