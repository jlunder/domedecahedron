#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

#include "domedecahedron.h"


typedef union {
    int32_t i;
    fix16_t f;
    void * p;
} effect_parameter_t;


static effect_parameter_t const effect_parameter_zero;


typedef struct {
    void * (* initialize)(void);
    void (* finalize)(void * state);
    void (* process)(void * state, color_t dest[DDH_TOTAL_VERTICES]);
    void (* set_parameter)(void * state, size_t id, effect_parameter_t value);
    effect_parameter_t (* get_parameter)(void * state, size_t id);
} effect_t;


extern effect_t const effect_add;
extern effect_t const effect_plasma_0;
extern effect_t const effect_rings_0;


extern void * effect_initialize(effect_t const * effect);
extern void effect_finalize(effect_t const * effect, void * state);
extern void effect_process(effect_t const * effect, void * state,
    color_t dest[DDH_TOTAL_VERTICES]);
extern void effect_set_parameter(effect_t const * effect, void * state,
    size_t id, effect_parameter_t value);
extern effect_parameter_t effect_get_parameter(effect_t const * effect,
    void * state, size_t id);

#endif
