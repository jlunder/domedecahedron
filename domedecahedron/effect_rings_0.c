#include "effect.h"

#include "effect_util.h"


void * effect_rings_0_initialize(void);
void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    color_t palette[3];
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
    effect_rings_0_state_t * state =
        (effect_rings_0_state_t *)malloc(sizeof (effect_rings_0_state_t));
    
    memset(state, 0, sizeof (effect_rings_0_state_t));
    
    for(size_t i = 0; i < 3; ++i) {
        state->palette[i] = eu_palette3_joe.colors[i];
    }
    
    return state;
}

void effect_rings_0_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t in0 = fix16_from_float(0.9f);
    fix16_t in1 = fix16_from_float(0.0f);
    fix16_t in2 = fix16_from_float(0.0f);
    
    effect_rings_0_state_t * state = (effect_rings_0_state_t *)voidp_state;
    color_t * sum_buf = eu_alloc_temp_buffer();
    color_t * temp_buf = eu_alloc_temp_buffer();
    color_t const * palette = NULL;
    color_t seq_buf[16];
    //fix16_t ring_a_t = state->time % fix16_from_int(8) / 8;
    fix16_t ring_bottom = fix16_from_float(-3.0f);
    fix16_t ring_height = fix16_from_float(3.5f);
    vector3_t ring_normal = vector3_make(0, 0, fix16_one);
    
    state->time += delta_time;
    
    //effect_rings_0_process_rainbow(state, sum_buf);
    
    switch((in0 * 5) / fix16_one) {
    default:
    case 0: palette = eu_palette3_dusk.colors; break;
    case 1: palette = eu_palette3_adam.colors; break;
    case 2: palette = eu_palette3_peter.colors; break;
    case 3: palette = eu_palette3_joe.colors; break;
    case 4: palette = eu_palette3_primaries.colors; break;
    }
    
    eu_color_seq_3(seq_buf, LENGTHOF(seq_buf), state->time, palette,
        LENGTHOF(state->palette));
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t pos = vector3_dot(ring_normal, ddh_vertex_coords_fix[i]) -
            ring_bottom;
        if(pos < 0) {
            pos = 0;
        }
        else if(pos >= ring_height) {
            pos = ring_height - 1;
        }
        temp_buf[i] = seq_buf[(pos * LENGTHOF(seq_buf) / ring_height)];
    }
    
    /*
    eu_bar(temp_buf, color_white, vector3_make(0, 0, fix16_one),
        fix16_mul(ring_a_t, fix16_from_float(3.5f)) - fix16_from_float(3.f),
        fix16_from_float(0.5f),
        fix16_from_float(0.1f));
    eu_add_buffer(sum_buf, 255, temp_buf, 255);
    */
    
    memcpy(sum_buf, temp_buf, sizeof state->buf);
    
    
    eu_temporal_iir_one_pole(state->buf, sum_buf, fix16_from_float(0.2));
    
    memcpy(buf, state->buf, sizeof state->buf);
    /*
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        sum_buf[i] = seq_buf[(pos * LENGTHOF(seq_buf) / ring_height)];
    }
    */
    eu_free_temp_buffer(temp_buf);
    eu_free_temp_buffer(sum_buf);
}


