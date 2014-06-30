#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

#include "domedecahedron.h"


typedef union {
    int32_t i;
    fix16_t f;
    void * p;
} effect_parameter_t;


static effect_parameter_t const effect_parameter_zero = {};


typedef struct {
    void * (* initialize)(void);
    void (* finalize)(void * state);
    void (* process)(void * state, fix16_t delta_time,
        color_t dest[DDH_TOTAL_VERTICES]);
    void (* set_parameter)(void * state, size_t id, effect_parameter_t value);
    effect_parameter_t (* get_parameter)(void * state, size_t id);
} effect_t;


typedef struct effect_instance_ effect_instance_t;


extern effect_t const effect_add;
extern effect_t const effect_twinkle;
extern effect_t const effect_glitch;
extern effect_t const effect_plasma_0;
extern effect_t const effect_rings_0;
extern effect_t const effect_dusk;


extern effect_instance_t * effect_initialize(effect_t const * instance);
extern void effect_finalize(effect_instance_t * instance);
extern void effect_process(effect_instance_t * instance, fix16_t delta_time,
    color_t dest[DDH_TOTAL_VERTICES]);
extern void effect_set_parameter(effect_instance_t * instance, size_t id,
    effect_parameter_t value);
extern effect_parameter_t effect_get_parameter(effect_instance_t * instance,
    size_t id);

#endif
