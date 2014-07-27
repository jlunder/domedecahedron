#include "effect.h"

#include "dais_input.h"
#include "effect_util.h"

static const vector3_t effect_controller_impulse_pos[2][2] = {
    {
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
    },
};

static const vector3_t effect_controller_impulse_pos_perp[2][2] = {
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
    {
        { fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
};


void * effect_controller_initialize(void);
void effect_controller_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

typedef struct {
    fix16_t time;
    fix16_t * lookup[DDH_TOTAL_VERTICES];
    vector3_t v;
    fix16_t r_v;
    vector3_t p;
    fix16_t r_p;
} effect_controller_state_t;

effect_t const effect_controller = {
    &effect_controller_initialize,
    &free,
    &effect_controller_process,
    NULL,
    NULL,
};

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
    state->p = vector3_add(state->p, vector3_scale(state->v, delta_time));
    state->r_p += fix16_mul(state->r_p, delta_time);
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            fix16_t a = vector3_dot(di_raw_motion_quadrants[i][j], effect_controller_impulse_pos[i][j]);
            fix16_t r_a = vector3_dot(di_raw_motion_quadrants[i][j], effect_controller_impulse_pos_perp[i][j]);
            
            state->v = vector3_add(state->v, vector3_scale(di_raw_motion_quadrants[i][j], fix16_mul(a, delta_time)));
            state->r_v += fix16_mul(r_a, delta_time);
        }
    }
    ddh_log("v: %6.4f,%6.4f / %6.4f\n", fix16_to_float(state->v.x), fix16_to_float(state->v.y), fix16_to_float(state->r_v));
    /**/
}


