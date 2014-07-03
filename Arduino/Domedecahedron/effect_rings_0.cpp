#include "effect.h"

#include "effect_util.h"


void * effect_rings_0_initialize(void);
void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    int32_t group_scramble;
    int32_t dodecahedron_scramble;
    int32_t vertex_scramble;
    struct {
        color_t color;
        vector3_t normal;
        fix16_t position;
        fix16_t velocity;
        fix16_t bar_size;
        fix16_t transition_size;
        fix16_t stop_position;
        bool running;
    } rings[4];
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
    
    for(size_t i = 0; i < LENGTHOF(rings_state->rings); ++i) {
        rings_state->rings[i].velocity = fix16_from_float(0.5f);
        rings_state->rings[i].bar_size = 0;
        rings_state->rings[i].transition_size = 0;
        rings_state->rings[i].position = fix16_from_int(-3) +
            eu_random() % fix16_from_int(6);
        rings_state->rings[i].running = true;
    }
    
    return rings_state;
}

void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_rings_0_state_t * rings_state = (effect_rings_0_state_t *)state;
    size_t running_count = 0;
    color_t * sum_buf = eu_alloc_temp_buffer();
    
    UNUSED(delta_time);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 127, 255);
    }
    
    for(size_t i = 0; i < LENGTHOF(rings_state->rings); ++i) {
        if(rings_state->rings[i].running) {
            rings_state->rings[i].position +=
                fix16_mul(rings_state->rings[i].velocity, delta_time);
            if(rings_state->rings[i].position +
                    rings_state->rings[i].transition_size >=
                    fix16_from_int(3)) {
                rings_state->rings[i].running = false;
            }
        }
        
        if(!rings_state->rings[i].running) {
            rings_state->rings[i].color = color_modulate_saturation(
                color_make(eu_random() & 0xFF, eu_random() & 0xFF,
                eu_random() & 0xFF), fix16_from_float(3.0f));
            rings_state->rings[i].normal = vector3_make(
                eu_random() % fix16_from_int(2) - fix16_one,
                eu_random() % fix16_from_int(2) - fix16_one,
                eu_random() % fix16_from_int(2) - fix16_one);
            if(rings_state->rings[i].normal.x == 0 &&
                    rings_state->rings[i].normal.y == 0 &&
                    rings_state->rings[i].normal.z == 0) {
                rings_state->rings[i].normal = vector3_make(0, 0, fix16_one);
            }
            else {
                rings_state->rings[i].normal =
                    vector3_normalize(rings_state->rings[i].normal);
            }
            rings_state->rings[i].velocity = fix16_from_float(0.5f);
            rings_state->rings[i].bar_size = fix16_from_float(0.25f);
            rings_state->rings[i].transition_size = fix16_from_float(0.125f);
            rings_state->rings[i].position = fix16_from_int(-3) -
                rings_state->rings[i].bar_size -
                rings_state->rings[i].transition_size;
            rings_state->rings[i].running = true;
        }
    }
    
    for(size_t i = 0; i < LENGTHOF(rings_state->rings); ++i) {
        if(rings_state->rings[i].running) {
            color_t * temp_buf = eu_alloc_temp_buffer();
            
            eu_bar(running_count == 0 ? sum_buf : temp_buf,
                rings_state->rings[i].color, rings_state->rings[i].normal,
                rings_state->rings[i].position,
                rings_state->rings[i].bar_size,
                rings_state->rings[i].transition_size);
            if(running_count > 0) {
                eu_add_buffer(sum_buf, 255, temp_buf, 255);
            }
            eu_free_temp_buffer(temp_buf);
            
            ++running_count;
        }
    }
    if((eu_random() % fix16_from_float(0.5f)) < delta_time) {
        switch(eu_random() % 16) {
        case 0:
            rings_state->group_scramble = eu_random() % DDH_TOTAL_GROUPS;
            break;
        case 1:
            rings_state->dodecahedron_scramble =
                eu_random() % DDH_DODECAHEDRONS_PER_GROUP;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            rings_state->vertex_scramble =
                eu_random() % DDH_VERTICES_PER_DODECAHEDRON;
            break;
        default:
            rings_state->group_scramble = 0;
            rings_state->dodecahedron_scramble = 0;
            rings_state->vertex_scramble = 0;
            break;
        }
    }
    
    eu_scramble(buf, sum_buf, rings_state->group_scramble,
        rings_state->dodecahedron_scramble, rings_state->vertex_scramble);
    eu_free_temp_buffer(sum_buf);
}


