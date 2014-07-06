#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"


void * effect_rings_0_initialize(void);
void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    fix16_t no_interaction_time;
    bool sweep_running;
    vector3_t sweep_normal;
    color_t sweep_color;
    fix16_t sweep_pos;
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
    
    return state;
}

void effect_rings_0_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_rings_0_state_t * state = (effect_rings_0_state_t *)voidp_state;
    
    fix16_t in0 = fix16_one - di_treated_z[0];
    fix16_t in1 = fix16_one - di_treated_z[1];
    fix16_t in2 = fix16_one - di_treated_z[2];
    
    color_t * sum_buf = eu_alloc_temp_buffer();
    color_t * temp_buf = eu_alloc_temp_buffer();
    
    fix16_t ring_bottom = fix16_from_float(-3.0f);
    fix16_t ring_height = fix16_from_float(3.5f);
    vector3_t ring_normal = vector3_make(0, 0, fix16_one);
    
    state->time += delta_time;
    
    memset(sum_buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
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
    
    if(in0 > 0) {
        color_t const * palette = NULL;
        color_t seq_buf[16];
        
        switch((in0 * 5) / fix16_one) {
        case 0: palette = eu_palette3_dusk.colors; break;
        case 1: palette = eu_palette3_adam.colors; break;
        case 2: palette = eu_palette3_peter.colors; break;
        case 3: palette = eu_palette3_joe.colors; break;
        default:
        case 4: palette = eu_palette3_primaries.colors; break;
        }
    
        eu_color_seq_3(seq_buf, LENGTHOF(seq_buf), state->time, palette,
            LENGTHOF(palette));
    
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
        
        eu_add_buffer(sum_buf, 255, temp_buf, 255);
    }
    
    {
        eu_bar(temp_buf, color_white, vector3_make(0, 0, fix16_one),
            fix16_mul(in1, fix16_from_float(3.5f)) - fix16_from_float(3.f),
            fix16_from_float(0.5f),
            fix16_from_float(0.1f));
        eu_add_buffer(sum_buf, 255, temp_buf, 255);
    }
    
    if(!state->sweep_running && in2 > 0) {
        state->sweep_running = true;
        do {
            state->sweep_normal.x = eu_random() % (fix16_one * 2) - fix16_one;
            state->sweep_normal.y = eu_random() % (fix16_one * 2) - fix16_one;
            state->sweep_normal.z = eu_random() % (fix16_one * 2) - fix16_one;
        } while(state->sweep_normal.x == 0 && state->sweep_normal.y == 0 &&
            state->sweep_normal.z == 0);
        state->sweep_normal = vector3_normalize(state->sweep_normal);
        state->sweep_pos = fix16_from_float(-2.0f);
        state->sweep_color = eu_lookup_palette3_random(&eu_palette3_adam);
    }
    
    if(state->sweep_running) {
        eu_bar(temp_buf, state->sweep_color, state->sweep_normal,
            state->sweep_pos, fix16_from_float(0.5f), fix16_from_float(0.1f));
        state->sweep_pos += fix16_mul(fix16_from_float(10.0f), delta_time);
        if(state->sweep_pos >= fix16_from_int(2)) {
            state->sweep_running = false;
        }
        
        eu_add_buffer(sum_buf, 255, temp_buf, 255);
    }
    
    eu_temporal_iir_one_pole(state->buf, sum_buf, fix16_from_float(0.2));
    memcpy(sum_buf, state->buf, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
    {
        size_t count = fix16_to_int(in1 * DDH_DODECAHEDRONS_PER_GROUP *
            DDH_VERTICES_PER_DODECAHEDRON);
        
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < count; ++j) {
                size_t dodecahedron = DDH_DODECAHEDRONS_PER_GROUP - 1 -
                    j / DDH_VERTICES_PER_DODECAHEDRON;
                size_t vertex = DDH_VERTICES_PER_DODECAHEDRON - 1 -
                    j % DDH_VERTICES_PER_DODECAHEDRON;
                temp_buf[ddh_group_dodecahedron_vertex_offsets[i][
                        dodecahedron][vertex]] =
                    eu_lookup_palette3_random(&eu_palette3_gold);
            }
        }
        eu_add_buffer(sum_buf, 255, temp_buf, 255);
    }
    
    memcpy(buf, sum_buf, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
    eu_free_temp_buffer(temp_buf);
    eu_free_temp_buffer(sum_buf);
}


