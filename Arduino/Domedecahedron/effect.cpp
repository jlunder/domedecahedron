#include "effect.h"

#include "effect_util.h"


void * effect_add_initialize(void);
void effect_add_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);
void effect_add_set_parameter(void * state, size_t id,
    effect_parameter_t value);
void * effect_twinkle_initialize(void);
void effect_twinkle_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);
void effect_twinkle_set_parameter(void * state, size_t id,
    effect_parameter_t value);
void * effect_glitch_initialize(void);
void effect_glitch_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);
void effect_glitch_set_parameter(void * state, size_t id,
    effect_parameter_t value);


#define EFFECT_ADD_MAX_SOURCES 4
#define EFFECT_ADD_PARAMETER_NUM_SOURCES 0
#define EFFECT_ADD_PARAMETER_SOURCE_N_BUF 0x100
#define EFFECT_ADD_PARAMETER_SOURCE_N_ALPHA 0x200


struct effect_instance_ {
    effect_t const * effect;
    void * state;
};

typedef struct {
    size_t num_sources;
    struct {
        color_t const * buf;
        uint8_t alpha;
    } sources[EFFECT_ADD_MAX_SOURCES];
} effect_add_state_t;

typedef struct {
    size_t temp;
} effect_twinkle_state_t;

typedef struct {
    size_t temp;
} effect_glitch_state_t;


effect_t const effect_add = {
    &effect_add_initialize,
    &free,
    &effect_add_process,
    &effect_add_set_parameter,
    NULL,
};

effect_t const effect_twinkle = {
    &effect_twinkle_initialize,
    &free,
    &effect_twinkle_process,
    &effect_twinkle_set_parameter,
    NULL,
};

effect_t const effect_glitch = {
    &effect_glitch_initialize,
    &free,
    &effect_glitch_process,
    &effect_glitch_set_parameter,
    NULL,
};


effect_instance_t * effect_initialize(effect_t const * effect)
{
    effect_instance_t * instance =
        (effect_instance_t *)malloc(sizeof (effect_instance_t));
    
    instance->effect = effect;
    
    if(effect->initialize != NULL) {
        assert(effect->finalize != NULL);
        instance->state = effect->initialize();
    }
    else {
        instance->state = NULL;
    }
    
    return instance;
}

void effect_finalize(effect_instance_t * instance)
{
    effect_t const * effect = instance->effect;
    assert(instance->state == NULL || effect->finalize != NULL);
    
    if(effect->finalize != NULL) {
        effect->finalize(instance->state);
    }
    free(instance);
}

void effect_process(effect_instance_t * instance, fix16_t delta_time,
    color_t dest[DDH_TOTAL_VERTICES])
{
    instance->effect->process(instance->state, delta_time, dest);
}

void effect_set_parameter(effect_instance_t * instance,
    size_t id, effect_parameter_t value)
{
    effect_t const * effect = instance->effect;
    
    if(effect->set_parameter != NULL) {
        effect->set_parameter(instance->state, id, value);
    }
}

effect_parameter_t effect_get_parameter(effect_instance_t * instance,
    size_t id)
{
    effect_t const * effect = instance->effect;
    
    if(effect->get_parameter != NULL) {
        return effect->get_parameter(instance->state, id);
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

void effect_add_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_add_state_t * add_state = (effect_add_state_t *)state;
    
    UNUSED(delta_time);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_black;
    }
    for(size_t i = 0; i < add_state->num_sources; ++i) {
        eu_add_buffer(buf, 255, add_state->sources[i].buf,
            add_state->sources[i].alpha);
    }
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

void * effect_twinkle_initialize(void)
{
    effect_twinkle_state_t * twinkle_state =
        (effect_twinkle_state_t *)malloc(sizeof (effect_twinkle_state_t));
    
    memset(twinkle_state, 0, sizeof (effect_twinkle_state_t));
    
    return twinkle_state;
}

void effect_twinkle_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_twinkle_state_t * twinkle_state = (effect_twinkle_state_t *)state;
    
    UNUSED(delta_time);
    UNUSED(twinkle_state);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_black;
    }
}

void effect_twinkle_set_parameter(void * state, size_t id,
    effect_parameter_t value)
{
    effect_twinkle_state_t * twinkle_state = (effect_twinkle_state_t *)state;
    
    UNUSED(twinkle_state);
    UNUSED(value);
    
    switch(id) {
    default:
        break;
    }
}

void * effect_glitch_initialize(void)
{
    effect_glitch_state_t * glitch_state =
        (effect_glitch_state_t *)malloc(sizeof (effect_glitch_state_t));
    
    memset(glitch_state, 0, sizeof (effect_glitch_state_t));
    
    return glitch_state;
}

void effect_glitch_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_glitch_state_t * glitch_state = (effect_glitch_state_t *)state;
    
    UNUSED(delta_time);
    UNUSED(glitch_state);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_black;
    }
}

void effect_glitch_set_parameter(void * state, size_t id,
    effect_parameter_t value)
{
    effect_glitch_state_t * glitch_state = (effect_glitch_state_t *)state;
    
    UNUSED(glitch_state);
    UNUSED(value);
    
    switch(id) {
    default:
        break;
    }
}

