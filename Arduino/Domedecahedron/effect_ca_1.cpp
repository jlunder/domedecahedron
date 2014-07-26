#include "effect.h"

#include "effect_util.h"


void * effect_ca_1_initialize(void);
void effect_ca_1_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    struct {
        bool running;
        eu_palette3_t const * pal;
        fix16_t seed_time;
        uint8_t age;
        uint8_t vertex_ages[DDH_DODECAHEDRONS_PER_GROUP][DDH_VERTICES_PER_DODECAHEDRON];
        size_t group;
    } seeds[10];
    struct {
        bool running;
        fix16_t pos;
        uint8_t pal_pos;
    } rings[3];
    color_t buf[DDH_TOTAL_VERTICES];
} effect_ca_1_state_t;

effect_t const effect_ca_1 = {
    &effect_ca_1_initialize,
    &free,
    &effect_ca_1_process,
    NULL,
    NULL,
};

void * effect_ca_1_initialize(void)
{
    effect_ca_1_state_t * state =
        (effect_ca_1_state_t *)malloc(sizeof (effect_ca_1_state_t));
    
    memset(state, 0, sizeof (*state));
    
    for(size_t i = 0; i < LENGTHOF(state->rings); ++i) {
        state->rings[i].pos = fix16_from_int(i);
    }
    
    return state;
}

void effect_ca_1_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_ca_1_state_t * state = (effect_ca_1_state_t *)voidp_state;
    
    UNUSED(buf);
    
    state->time += delta_time;
    
    while(state->time > fix16_from_float(0.2f)) {
        state->time -= fix16_from_float(0.2f);
        for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
            if(!state->seeds[i].running) {
                if(eu_random() % 10 == 0) {
                    switch(eu_random() % 6) {
                    case 0: state->seeds[i].pal = &eu_palette3_dusk; break;
                    case 1: state->seeds[i].pal = &eu_palette3_adam; break;
                    case 2: state->seeds[i].pal = &eu_palette3_peter; break;
                    case 3: state->seeds[i].pal = &eu_palette3_joe; break;
                    case 4: state->seeds[i].pal = &eu_palette3_primaries; break;
                    case 5: state->seeds[i].pal = &eu_palette3_gold; break;
                    }
                    state->seeds[i].seed_time = state->time;
                    state->seeds[i].age = 20;
                    memset(state->seeds[i].vertex_ages, 0, sizeof state->seeds[i].vertex_ages);
                    state->seeds[i].vertex_ages[
                        eu_random() % DDH_DODECAHEDRONS_PER_GROUP][
                        eu_random() % DDH_VERTICES_PER_DODECAHEDRON] = state->seeds[i].age;
                    state->seeds[i].group = eu_random() % DDH_TOTAL_GROUPS;
                    state->seeds[i].running = true;
                }
                else {
                    continue;
                }
            }
            --state->seeds[i].age;
            if(state->seeds[i].age == 0) {
                state->seeds[i].running = false;
            }
            for(size_t j = 0; j < DDH_DODECAHEDRONS_PER_GROUP; ++j) {
                for(size_t k = 0; k < DDH_VERTICES_PER_DODECAHEDRON; ++k) {
                    size_t o = ddh_group_dodecahedron_vertex_offsets[
                        state->seeds[i].group][j][k];
                    uint8_t age = state->seeds[i].vertex_ages[j][k];
                    for(size_t l = 0;
                            l < LENGTHOF(ddh_dodecahedron_vertex_adjacency[
                                k]);
                            ++l) {
                        size_t av = ddh_dodecahedron_vertex_adjacency[k][l];
                        if(state->seeds[i].vertex_ages[j][av] < age) {
                            state->seeds[i].vertex_ages[j][av] = age - 1;
                        }
                    }
                    for(size_t l = 0;
                            l < ddh_dodecahedron_adjacency[o].num_offsets;
                            ++l) {
                        size_t av = ddh_dodecahedron_adjacency[o].offsets[l];
                        size_t g = ddh_light_group[av];
                        size_t d = ddh_light_dodecahedron[av];
                        size_t v = ddh_light_vertex[av];
                        if(g == state->seeds[i].group && 
                                state->seeds[i].vertex_ages[d][v] < age) {
                            state->seeds[i].vertex_ages[d][v] = age - 1;
                        }
                    }
                }
            }
        }
    }
    
    memset(buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    
    for(size_t i = 0; i < LENGTHOF(state->seeds); ++i) {
        color_t c;
    
        if(!state->seeds[i].running) {
            continue;
        }
        c = eu_lookup_palette3(state->seeds[i].age * 10, state->seeds[i].pal);
        for(size_t j = 0; j < DDH_DODECAHEDRONS_PER_GROUP; ++j) {
            for(size_t k = 0; k < DDH_VERTICES_PER_DODECAHEDRON; ++k) {
                size_t o = ddh_group_dodecahedron_vertex_offsets[
                    state->seeds[i].group][j][k];
                if(state->seeds[i].vertex_ages[j][k] == state->seeds[i].age) {
                    buf[o] = color_add_sat(buf[o], c);
                }
            }
        }
    }
    
    eu_temporal_iir_one_pole(state->buf, buf, fix16_from_float(0.1));
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_modulate_saturation(state->buf[i], fix16_from_int(3));
    }
    
    /*(
    memset(buf, 0, sizeof (color_t) * DDH_TOTAL_VERTICES);
    for(size_t i = 0; i < LENGTHOF(state->rings); ++i) {
        color_t * tb = eu_alloc_temp_buffer();
        state->rings[i].pos += delta_time / 100;
        if(state->rings[i].pos > fix16_from_int(6)) {
            state->rings[i].pos -= fix16_from_int(6);
            state->rings[i].pal_pos = eu_random() % 256;
        }
        eu_bar(tb, eu_lookup_palette3(state->rings[i].pal_pos, &eu_palette3_joe),
            vector3_make(0, 0, 1), fix16_from_int(0),
                fix16_from_float(0.2f), fix16_from_float(0.05f));
        eu_add_buffer(buf, 255, tb, 255);
        eu_free_temp_buffer(tb);
    }
    */
}
