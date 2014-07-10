#include "effect.h"

#include "effect_util.h"


void * effect_ca_0_initialize(void);
void effect_ca_0_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    struct {
        size_t pos;
        color_t c;
    } seeds[10];
    color_t buf[DDH_TOTAL_VERTICES];
} effect_ca_0_state_t;

effect_t const effect_ca_0 = {
    &effect_ca_0_initialize,
    &free,
    &effect_ca_0_process,
    NULL,
    NULL,
};

void * effect_ca_0_initialize(void)
{
    effect_ca_0_state_t * state =
        (effect_ca_0_state_t *)malloc(sizeof (effect_ca_0_state_t));
    
    memset(state, 0, sizeof (*state));
    
    for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
        state->seeds[i].pos = eu_random() % DDH_TOTAL_VERTICES;
        state->seeds[i].c = eu_lookup_palette3_random(&eu_palette3_adam);
        
        state->seeds[i].c = color_make(state->seeds[i].c.r * 5 / 8,
            state->seeds[i].c.g * 5 / 8, state->seeds[i].c.b * 5 / 8);
    }
    
    return state;
}

void effect_ca_0_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_ca_0_state_t * state = (effect_ca_0_state_t *)voidp_state;
    
    state->time += delta_time;
    
    while(state->time > fix16_from_float(0.01f)) {
        state->time -= fix16_from_float(0.01f);
        
        for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
            size_t o = state->seeds[i].pos;
            size_t g = ddh_light_group[o];
            size_t d = ddh_light_dodecahedron[o];
            size_t v = ddh_light_vertex[o];
            size_t ai = eu_random() %
                (LENGTHOF(ddh_dodecahedron_vertex_adjacency[0]) +
                    ddh_dodecahedron_adjacency[o].num_offsets);
            size_t ao = 0;
            
            if(ai < LENGTHOF(ddh_dodecahedron_vertex_adjacency[0])) {
                size_t av = ddh_dodecahedron_vertex_adjacency[v][ai];
                ao = ddh_group_dodecahedron_vertex_offsets[g][d][av];
            }
            else {
                ao = ddh_dodecahedron_adjacency[o].offsets[
                    ai - LENGTHOF(ddh_dodecahedron_vertex_adjacency[0])];
            }
            
            state->seeds[i].pos = ao;
        }
    }
    
    memset(buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
    for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
        state->buf[state->seeds[i].pos] =
            color_add_sat(state->buf[state->seeds[i].pos], state->seeds[i].c);
        
        /*
        if(eu_random() % fix16_from_float(5.0f) < delta_time) {
            
        }
        */
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        size_t g = ddh_light_group[i];
        size_t d = ddh_light_dodecahedron[i];
        size_t v = ddh_light_vertex[i];
        size_t avg_count = 2 + 3 + ddh_dodecahedron_adjacency[i].num_offsets + 1;
        int32_t cr = state->buf[i].r * 2;
        int32_t cg = state->buf[i].r * 2;
        int32_t cb = state->buf[i].r * 2;
        
        for(size_t j = 0; j < LENGTHOF(ddh_dodecahedron_vertex_adjacency[0]);
                ++j) {
            size_t av = ddh_dodecahedron_vertex_adjacency[v][j];
            size_t ao = ddh_group_dodecahedron_vertex_offsets[g][d][av];
            cr += state->buf[ao].r;
            cg += state->buf[ao].g;
            cb += state->buf[ao].b;
        }
        
        for(size_t j = 0;
                j < ddh_dodecahedron_adjacency[i].num_offsets; ++j) {
            size_t ao = ddh_dodecahedron_adjacency[i].offsets[j];
            cr += state->buf[ao].r;
            cg += state->buf[ao].g;
            cb += state->buf[ao].b;
        }
        
        buf[i] = color_make(cr / avg_count, cg / avg_count, cb / avg_count);
    }
    
    for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
        buf[state->seeds[i].pos] = state->seeds[i].c;
    }
    
    eu_temporal_iir_one_pole(state->buf, buf, fix16_from_float(0.1));
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_modulate_saturation(state->buf[i], fix16_from_int(3));
    }
}
