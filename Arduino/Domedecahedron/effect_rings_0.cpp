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
    
    fix16_t creep_time;
    size_t creep_target;
    size_t creep_pos;
    
    fix16_t random_walk_time;
    size_t random_walk_cur_offset;
    size_t random_walk_vertex;
    size_t random_walk_offsets[DDH_VERTICES_PER_DODECAHEDRON];
    
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
        size_t in1_count = fix16_to_int(in1 * DDH_DODECAHEDRONS_PER_GROUP *
            DDH_VERTICES_PER_DODECAHEDRON);
        size_t creep_pos = state->creep_pos;
        
        if(in1_count > state->creep_target) {
            state->creep_target = in1_count;
        }
        state->creep_time += delta_time;
        if(state->creep_time >= fix16_from_float(0.1f)) {
            state->creep_time -= fix16_from_float(0.1f);
            if(creep_pos < state->creep_target) {
                ++state->creep_pos;
            }
            else if(creep_pos > state->creep_target) {
                --state->creep_pos;
            }
            if(creep_pos >= state->creep_target) {
                state->creep_target = in1_count;
            }
            creep_pos = state->creep_pos;
        }
        
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < creep_pos; ++j) {
                if(j >= creep_pos - 5) {
                    size_t dodecahedron = DDH_DODECAHEDRONS_PER_GROUP - 1 -
                        j / DDH_VERTICES_PER_DODECAHEDRON;
                    size_t vertex = DDH_VERTICES_PER_DODECAHEDRON - 1 -
                        j % DDH_VERTICES_PER_DODECAHEDRON;
                    size_t offset = ddh_group_dodecahedron_vertex_offsets[i][
                            dodecahedron][vertex];
                    sum_buf[offset] = color_add_sat(sum_buf[offset],
                        eu_lookup_palette3_random(&eu_palette3_gold));
                }
            }
        }
    }
    
    {
        color_t seq_buf[DDH_VERTICES_PER_DODECAHEDRON];
        
        state->random_walk_time += delta_time;
        if(state->random_walk_time > fix16_from_float(0.5f)) {
            state->random_walk_time -= fix16_from_float(0.5f);
            state->random_walk_vertex = ddh_dodecahedron_vertex_adjacency[
                state->random_walk_vertex][eu_random() % 3];
            state->random_walk_offsets[state->random_walk_vertex] =
                state->random_walk_cur_offset;
            state->random_walk_cur_offset =
                (state->random_walk_cur_offset + 1) %
                DDH_VERTICES_PER_DODECAHEDRON;
        }
        
        eu_color_seq_0(seq_buf, LENGTHOF(seq_buf), state->time / 2,
            eu_palette3_dusk.colors, LENGTHOF(eu_palette3_dusk.colors));
        for(size_t i = 0; i < DDH_VERTICES_PER_DODECAHEDRON; ++i) {
            size_t offset = ddh_group_dodecahedron_vertex_offsets[
                DDH_TOTAL_GROUPS][0][i];
            sum_buf[offset] = seq_buf[state->random_walk_offsets[i]];
        }
        sum_buf[ddh_group_dodecahedron_vertex_offsets[
            DDH_TOTAL_GROUPS][0][state->random_walk_vertex]] = color_white;
    }
    
    eu_temporal_iir_one_pole(state->buf, sum_buf, fix16_from_float(0.2));
    memcpy(buf, state->buf, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
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
        if(state->sweep_pos >= fix16_from_int(2)) {
            if(in2 == 0) {
                state->sweep_running = false;
            }
        }
        else {
            state->sweep_pos +=
                fix16_mul(fix16_from_float(10.0f), delta_time);
        }
        
        eu_add_buffer(buf, 255, temp_buf, 255);
    }
    
    
    eu_free_temp_buffer(temp_buf);
    eu_free_temp_buffer(sum_buf);
}


