#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"

static const vector2_t effect_controller_impulse_pos[2][2] = {
    {
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two},
    },
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two},
        { fix16_sqrt_two_two, -fix16_sqrt_two_two},
    },
};

static const vector2_t effect_controller_impulse_pos_perp[2][2] = {
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two},
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two},
    },
    {
        { fix16_sqrt_two_two, -fix16_sqrt_two_two},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two},
    },
};


void * effect_controller_initialize(void);
void effect_controller_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    fix16_t * lookup[DDH_TOTAL_VERTICES];
    vector2_t v;
    fix16_t r_v;
    vector2_t p;
    fix16_t r_p;
} effect_controller_state_t;

effect_t const effect_controller = {
    &effect_controller_initialize,
    &free,
    &effect_controller_process,
    NULL,
    NULL,
};

void effect_controller_process_motion(effect_controller_state_t * state,
    fix16_t delta_time);

void * effect_controller_initialize(void)
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)malloc(sizeof (effect_controller_state_t));
    
    memset(state, 0, sizeof (effect_controller_state_t));
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        int32_t row =
            (int32_t)((atan2f(ddh_vertex_coords[i].y,
                ddh_vertex_coords[i].x) + PI_F) * 4.0f / TWO_PI_F);
        int32_t col = (int32_t)((ddh_vertex_coords[i].z + 2.0f) * 2.0f);
        row = ((row < 0 ? 0 : row >= 4 ? 3 : row) + 3) % 4;
        col = col < 0 ? 0 : col >= 4 ? 3 : col;
        
        state->lookup[i] = &di_treated_z[row][col];
    }
    
    return state;
}

void effect_controller_process(void * void_state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES])
{
    effect_controller_state_t * state =
        (effect_controller_state_t *)void_state;
    //size_t running_count = 0;
    //color_t * sum_buf = eu_alloc_temp_buffer();
    
    UNUSED(state);
    UNUSED(delta_time);
    
    effect_controller_process_motion(state, delta_time);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        buf[i] = color_make(fix16_to_int(255 * *state->lookup[i]), 0, 0);
    }
    
    /*
    ddh_log("motion:");
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            ddh_log("%s ",
                di_raw_motion_quadrants[i][j].x < fix16_from_float(-0.50f) ? "<-*  " :
                di_raw_motion_quadrants[i][j].x < fix16_from_float(-0.10f) ? " <*  " :
                di_raw_motion_quadrants[i][j].x < fix16_from_float( 0.10f) ? "  *  " :
                di_raw_motion_quadrants[i][j].x < fix16_from_float( 0.50f) ? "  *> " :
                                                                             "  *->");
        }
    }
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            ddh_log("%s ",
                di_raw_motion_quadrants[i][j].y < fix16_from_float(-0.50f) ? "<-*  " :
                di_raw_motion_quadrants[i][j].y < fix16_from_float(-0.10f) ? " <*  " :
                di_raw_motion_quadrants[i][j].y < fix16_from_float( 0.10f) ? "  *  " :
                di_raw_motion_quadrants[i][j].y < fix16_from_float( 0.50f) ? "  *> " :
                                                                             "  *->");
        }
    }
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            ddh_log("%s ",
                di_raw_motion_quadrants[i][j].z < fix16_from_float(-0.50f) ? "<-*  " :
                di_raw_motion_quadrants[i][j].z < fix16_from_float(-0.10f) ? " <*  " :
                di_raw_motion_quadrants[i][j].z < fix16_from_float( 0.10f) ? "  *  " :
                di_raw_motion_quadrants[i][j].z < fix16_from_float( 0.50f) ? "  *> " :
                                                                             "  *->");
        }
    }
    ddh_log("\n");
    /**/
    ///*
    // todo add 0.5 * a * t * t
    /**/
}

void effect_controller_process_motion(effect_controller_state_t * state,
    fix16_t delta_time)
{
    vector2_t a = vector2_make(0, 0);
    fix16_t r_a = 0;
    
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            vector2_t motion = vector2_make(di_raw_motion_quadrants[i][j].x,
                di_raw_motion_quadrants[i][j].y);
            fix16_t q_a =
                vector2_dot(motion, effect_controller_impulse_pos[i][j]);
            fix16_t q_r_a =
                vector2_dot(motion, effect_controller_impulse_pos_perp[i][j]);
            
            a = vector2_add(a,
                vector2_scale(effect_controller_impulse_pos[i][j], q_a));
            r_a += q_r_a;
        }
    }
    
    state->p = vector2_add(state->p, vector2_add(
        vector2_scale(state->v, delta_time),
        vector2_scale(a, fix16_sq(delta_time) / 2)));
    state->r_p += fix16_mul(state->r_p, delta_time) +
        fix16_mul(r_a, fix16_sq(delta_time) / 2);
    
    state->v = vector2_add(state->v, vector2_scale(a, delta_time));
    state->r_v += fix16_mul(r_a, delta_time);
    
    // pretty sure this is mathematically off but I don't want to integrate
    {
        fix16_t f_a = fix16_mul(fix16_from_float(0.05f), delta_time);
        fix16_t f_r_a = fix16_mul(fix16_from_float(0.025f), delta_time);
        fix16_t v_mag = fix16_sqrt(vector2_lengthsq(state->v));
        
        if(v_mag <= f_a) {
            state->v = vector2_make(0, 0);
        }
        else {
            vector2_t dir = vector2_normalize(state->v);
            vector2_t delta_v = vector2_scale(dir, f_a);
            state->v = vector2_sub(state->v, delta_v);
            assert(fix16_sqrt(vector2_lengthsq(state->v)) <= v_mag);
        }
        
        if(abs(state->r_v) < f_r_a) {
            state->r_v = 0;
        }
        else {
            if(state->r_v > 0) {
                state->r_v -= f_r_a;
            }
            else {
                state->r_v += f_r_a;
            }
        }
    }
    
    ddh_log("v: %6.4f,%6.4f / %6.4f\n", fix16_to_float(state->v.x), fix16_to_float(state->v.y), fix16_to_float(state->r_v));
}


