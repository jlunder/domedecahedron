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
void effect_plasma_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);
void * effect_rings_0_initialize(void);
void effect_rings_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);


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

effect_t const effect_plasma_0 = {
    NULL,
    NULL,
    &effect_plasma_0_process,
    NULL,
    NULL,
};

effect_t const effect_rings_0 = {
    &effect_rings_0_initialize,
    &free,
    &effect_rings_0_process,
    NULL,
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

void effect_plasma_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t t =
        fix16_from_float((float)(ddh_total_ns % (1LL << 48)) / 1.0e9f);
    color_t ca = color_make(
        fix16_mul(fix16_sin(fix16_mul(t / 20, fix16_two_pi)) +
            fix16_from_int(1), 127),
        fix16_mul(fix16_cos(fix16_mul(t / 30, fix16_two_pi)) +
            fix16_from_int(1), 127),
        fix16_mul(fix16_sin(fix16_mul(t / 70, fix16_two_pi)) +
            fix16_from_int(1), 63));
    color_t cb = color_make(
        fix16_mul(fix16_sin(fix16_mul(t / 20, fix16_two_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 127),
        fix16_mul(fix16_cos(fix16_mul(t / 30, fix16_two_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 127),
        fix16_mul(fix16_sin(fix16_mul(t / 70, fix16_two_pi) +
            fix16_from_int(3)) + fix16_from_int(1), 63));
    vector3_t pa = vector3_make(
        2 * fix16_sin(fix16_mul(t / 8, fix16_two_pi)) - fix16_from_int(3),
        2 * fix16_cos(fix16_mul(t / 8, fix16_two_pi)) - fix16_from_int(1),
        fix16_from_int(-2));
    vector3_t pb = vector3_make(
        1 * fix16_sin(fix16_mul(t / 9, fix16_two_pi) + fix16_from_int(19)),
        fix16_from_int(2),
        fix16_from_int(-2));
    vector3_t pc = vector3_make(
        fix16_from_int(6),
        4 * fix16_sin(fix16_mul(t / 25, fix16_two_pi) +
            fix16_from_int(7)) + fix16_from_int(2),
        fix16_from_int(-2));

    int32_t l;
    
    UNUSED(state);
    UNUSED(delta_time);
    
    ca = color_modulate_saturation(ca, fix16_from_float(4.0f));
    cb = color_modulate_saturation(cb, fix16_from_float(4.0f));
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        fix16_t plasma_a =
            eu_sin_dist3(pa,
                ddh_vertex_coords_fix[i],
                fix16_pi, 0) + fix16_from_int(1);
        
        fix16_t plasma_b =
            eu_sin_dist3(pb,
                ddh_vertex_coords_fix[i],
                fix16_pi * 13 / 7,
                0) + fix16_from_int(1);
        
        fix16_t plasma_c =
            eu_sin_dist3(pc,
                ddh_vertex_coords_fix[i],
                fix16_pi * 3 / 5,
                0) + fix16_from_int(1);
        
        l = fix16_to_int(
            fix16_mul((plasma_a + plasma_b + plasma_c),
            fix16_from_int(128) / 3));
        
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


void * effect_dusk_initialize(void);
void effect_dusk_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    size_t stars_pos[100];
    vector3_t auroras_pos[6];
    color_t buf[DDH_TOTAL_VERTICES];
} effect_dusk_state_t;

effect_t const effect_dusk = {
    &effect_dusk_initialize,
    &free,
    &effect_dusk_process,
    NULL,
    NULL,
};

void * effect_dusk_initialize(void)
{
    effect_dusk_state_t * state =
        (effect_dusk_state_t *)malloc(sizeof (effect_dusk_state_t));
    
    memset(state, 0, sizeof (*state));
    
    for(size_t i = 0; i < LENGTHOF(state->stars_pos); ++i) {
        state->stars_pos[i] = eu_random() % DDH_TOTAL_VERTICES;
    }
    
    for(size_t i = 0; i < LENGTHOF(state->auroras_pos); ++i) {
        state->auroras_pos[i].x = (eu_random() % fix16_one) * 2 - fix16_one;
        state->auroras_pos[i].y =
            (eu_random() % fix16_one) * 6 - fix16_from_int(3);
        state->auroras_pos[i].z = fix16_from_float(-0.5);
    }
    
    return state;
}

void effect_dusk_process(void * voidp_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    fix16_t auroras_v = fix16_from_float(0.75f);
    effect_dusk_state_t * state = (effect_dusk_state_t *)voidp_state;
    fix16_t t = state->time / 300;
    vector3_t up_vec = vector3_normalize(vector3_make(0,
        fix16_from_float(0.3f), fix16_one));
    fix16_t bias;
    
    if(t > fix16_one) {
        t = fix16_one;
    }
    
    up_vec.x = fix16_mul(up_vec.x, fix16_from_float(0.35f));
    up_vec.y = fix16_mul(up_vec.y, fix16_from_float(0.35f));
    up_vec.z = fix16_mul(up_vec.z, fix16_from_float(0.35f));
    
    bias = fix16_from_float(0.8f) + t / 2;
    
    state->time += delta_time;
    
    for(size_t i = 0; i < LENGTHOF(state->auroras_pos); ++i) {
        state->auroras_pos[i].y -= fix16_mul(delta_time, auroras_v);
        if(state->auroras_pos[i].y < fix16_from_int(-3)) {
            state->auroras_pos[i].y += fix16_from_int(6);
        }
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        vector3_t vc = ddh_vertex_coords_fix[i];
        fix16_t pos = vector3_dot(up_vec, vc) + bias;
        int32_t pal_pos = fix16_to_int(pos * 256);
        color_t c = eu_lookup_palette3(
            pal_pos > 255 ? 255 : (pal_pos < 0 ? 0 : pal_pos),
            &eu_palette3_dusk);
        
        if(ddh_vertex_coords_fix[i].z > fix16_from_int(-1)) {
            fix16_t al = 0;
            for(size_t j = 0; j < LENGTHOF(state->auroras_pos); ++j) {
                fix16_t dsq = vector3_distsq(state->auroras_pos[j], vc);
                if(dsq < fix16_one) {
                    al += fix16_one - dsq;
                }
            }
            
            if(al > fix16_one) {
                al = fix16_one;
            }
            
            c = color_add_sat(c, color_make(0, fix16_to_int(al * 95), 0));
        }
        buf[i] = c;
    }
    
    for(size_t i = 0; i < LENGTHOF(state->stars_pos); ++i) {
        uint_fast8_t l = fix16_to_int(t * (eu_random() % 256));
        
        if(eu_random() % fix16_from_int(30) < delta_time) {
            state->stars_pos[i] = eu_random() % DDH_TOTAL_VERTICES;
        }
        
        buf[state->stars_pos[i]] = color_add_sat(buf[state->stars_pos[i]],
            color_make(l, l, l));
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        uint32_t r = (uint32_t)state->buf[i].r * 15 + (uint32_t)buf[i].r * 1;
        uint32_t g = (uint32_t)state->buf[i].g * 15 + (uint32_t)buf[i].g * 1;
        uint32_t b = (uint32_t)state->buf[i].b * 15 + (uint32_t)buf[i].b * 1;
        state->buf[i] = buf[i] = color_make(r / 16, g / 16, b / 16);
    }
}
