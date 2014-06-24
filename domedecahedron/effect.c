#include "effect.h"

#include "effect_util.h"


void * effect_add_initialize(void);
void effect_add_process(void * state, color_t buf[DDH_TOTAL_VERTICES]);
void effect_add_set_parameter(void * state, size_t id,
    effect_parameter_t value);
void effect_plasma_0_process(void * state, color_t buf[DDH_TOTAL_VERTICES]);
void effect_rings_0_process(void * state, color_t buf[DDH_TOTAL_VERTICES]);


#define EFFECT_ADD_MAX_SOURCES 4
#define EFFECT_ADD_PARAMETER_NUM_SOURCES 0
#define EFFECT_ADD_PARAMETER_SOURCE_N_BUF 0x100
#define EFFECT_ADD_PARAMETER_SOURCE_N_ALPHA 0x200


typedef struct {
    size_t num_sources;
    struct {
        color_t const * buf;
        uint8_t alpha;
    } sources[EFFECT_ADD_MAX_SOURCES];
} effect_add_state_t;


typedef struct {
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


effect_t const effect_add = {
    &effect_add_initialize,
    &free,
    &effect_add_process,
    &effect_add_set_parameter,
    NULL,
};

effect_t const effect_plasma_0 = {
    NULL,
    NULL,
    &effect_plasma_0_process,
    NULL,
    NULL,
};

effect_t const effect_rings_0 = {
    NULL,
    NULL,
    &effect_rings_0_process,
    NULL,
    NULL,
};


void * effect_initialize(effect_t const * effect)
{
    if(effect->initialize != NULL) {
        assert(effect->finalize != NULL);
        return effect->initialize();
    }
    else {
        return NULL;
    }
}

void effect_finalize(effect_t const * effect, void * state)
{
    assert(state == NULL || effect->finalize != NULL);
    
    if(effect->finalize != NULL) {
        effect->finalize(state);
    }
}

void effect_process(effect_t const * effect, void * state,
    color_t dest[DDH_TOTAL_VERTICES])
{
    effect->process(state, dest);
}

void effect_set_parameter(effect_t const * effect, void * state,
    size_t id, effect_parameter_t value)
{
    if(effect->set_parameter != NULL) {
        effect->set_parameter(state, id, value);
    }
}

effect_parameter_t effect_get_parameter(effect_t const * effect, void * state,
    size_t id)
{
    if(effect->get_parameter != NULL) {
        return effect->get_parameter(state, id);
    }
    else {
        return effect_parameter_zero;
    }
}

void * effect_add_initialize(void)
{
    effect_add_state_t * add_state =
        (effect_add_state_t *)malloc(sizeof (effect_add_state_t));
    
    memset(add_state, 0, sizeof (effect_add_state_t));
    add_state->num_sources = 0;
    
    return add_state;
}

void effect_add_process(void * state, color_t buf[DDH_TOTAL_VERTICES])
{
    effect_add_state_t * add_state = (effect_add_state_t *)state;
}

void effect_add_set_parameter(void * state, size_t id,
    effect_parameter_t value)
{
    effect_add_state_t * add_state = (effect_add_state_t *)state;
    
    switch(id) {
    case EFFECT_ADD_PARAMETER_NUM_SOURCES:
        add_state->num_sources = value.i;
        break;
    default:
        if(id >= EFFECT_ADD_PARAMETER_SOURCE_N_BUF &&
                id < EFFECT_ADD_PARAMETER_SOURCE_N_BUF +
                EFFECT_ADD_MAX_SOURCES) {
            add_state->sources[id - EFFECT_ADD_PARAMETER_SOURCE_N_BUF].buf =
                (color_t const *)value.p;
        }
        else if(id >= EFFECT_ADD_PARAMETER_SOURCE_N_ALPHA &&
                id < EFFECT_ADD_PARAMETER_SOURCE_N_ALPHA +
                EFFECT_ADD_MAX_SOURCES) {
            add_state->sources[id - EFFECT_ADD_PARAMETER_SOURCE_N_BUF].alpha =
                value.i;
        }
        break;
    }
}

void effect_plasma_0_process(void * state, color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t t =
        fix16_from_float((float)(ddh_total_ns % (1LL << 48)) / 1.0e9f);
    color_t ca = color_make(
        fix16_mul(fix16_sin(fix16_mul(t / 20, 2 * fix16_pi)) +
            fix16_from_int(1), 127),
        fix16_mul(fix16_cos(fix16_mul(t / 30, 2 * fix16_pi)) +
            fix16_from_int(1), 127),
        fix16_mul(fix16_sin(fix16_mul(t / 70, 2 * fix16_pi)) +
            fix16_from_int(1), 63));
    color_t cb = color_make(
        fix16_mul(fix16_sin(fix16_mul(t / 20, 2 * fix16_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 127),
        fix16_mul(fix16_cos(fix16_mul(t / 30, 2 * fix16_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 127),
        fix16_mul(fix16_sin(fix16_mul(t / 70, 2 * fix16_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 63));
    int32_t l;
    
    ca = color_modulate_saturation(ca, fix16_from_float(7.0f));
    cb = color_modulate_saturation(cb, fix16_from_float(7.0f));
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t plasma_a =
            eu_sin_dist3(
                vector3_make(
                    2 * fix16_sin(fix16_mul(t / 8, 2 * fix16_pi)) -
                        fix16_from_int(3),
                    2 * fix16_cos(fix16_mul(t / 8, 2 * fix16_pi)) -
                        fix16_from_int(1),
                    fix16_from_int(-2)),
                ddh_vertex_coords_fix[i],
                fix16_pi,
                0) + fix16_from_int(1);
        
        fix16_t plasma_b =
            eu_sin_dist3(
                vector3_make(
                    1 * fix16_sin(fix16_mul(t / 9, 2 * fix16_pi) +
                        fix16_from_int(19)),
                    fix16_from_int(2),
                    fix16_from_int(-2)),
                ddh_vertex_coords_fix[i],
                fix16_pi * 13 / 7,
                0) + fix16_from_int(1);
        
        fix16_t plasma_c =
            eu_sin_dist3(
                vector3_make(
                    fix16_from_int(6),
                    4 * fix16_sin(fix16_mul(t / 25, 2 * fix16_pi) +
                        fix16_from_int(7)) + fix16_from_int(2),
                    fix16_from_int(-2)),
                ddh_vertex_coords_fix[i],
                fix16_pi * 3 / 5,
                0) + fix16_from_int(1);
        
        l = fix16_to_int(
            fix16_mul((plasma_a + plasma_b + plasma_c) / 3,
            fix16_from_int(127)));
        
        if((l > 250) || (l < 2)) {
            buf[i] = ((l & 1) == 0) ? color_black : color_white;
        }
        else {
            buf[i] = color_make(
                (l * ca.r + (255 - l) * cb.r) / 256,
                (l * ca.g + (255 - l) * cb.g) / 256,
                (l * ca.b + (255 - l) * cb.b) / 256);
        }
    }
}


void effect_rings_0_process(void * state, color_t buf[DDH_TOTAL_VERTICES])
{
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(0, 127, 255);
    }
}


