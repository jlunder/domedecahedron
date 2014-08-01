#include "dais_input.h"


static int32_t const di_motion_lookup[16] = {
    0, 1, -1, 0, -1, 0, 1, 1, 1, -1, 0, -1, 0, -1, 1, 0,
};
static int32_t const di_detection_filter_k = (int32_t)(256 * 0.7);
static int32_t const di_detection_filter_one_minus_k =
  256 - di_detection_filter_k;
static int32_t const di_detection_threshold = 32768;

static const vector3_t di_impulse_pos[2][2] = {
    {
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
    },
};

static const vector3_t di_impulse_pos_perp[2][2] = {
    {
        {-fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        {-fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
    {
        { fix16_sqrt_two_two, -fix16_sqrt_two_two, 0},
        { fix16_sqrt_two_two,  fix16_sqrt_two_two, 0},
    },
};


vector3_t di_motion_quadrants[2][2];

vector3_t di_rotation;
fix16_t di_flat_rotation;
fix16_t di_flat_rotation_v;
vector3_t di_translation;
vector3_t di_translation_v;

fix16_t di_last_treated_z[4][4];
fix16_t di_treated_z[4][4];
bool di_last_detect[4][4];
bool di_detect[4][4];

vector3_t di_raw_motion_quadrants[2][2];


int32_t di_scale_raw_dais(int32_t raw);
int32_t di_motion_detect(size_t r0, size_t c0, size_t r1, size_t c1);
void di_process_inertia(fix16_t delta_time);


void di_initialize(void)
{
    memset(di_last_detect, 0, sizeof di_last_detect);
    
    di_rotation = vector3_make(0, 0, 0);
    di_flat_rotation = 0;
    di_flat_rotation_v = 0;
    di_translation = vector3_make(0, 0, 0);
    di_translation_v = vector3_make(0, 0, 0);
}


void di_process(fix16_t delta_time)
{
    memcpy(di_last_detect, di_detect, sizeof di_last_detect);
    memcpy(di_last_treated_z, di_treated_z, sizeof di_last_treated_z);
    
    for(size_t i = 0; i < 4; ++i) {
        for(size_t j = 0; j < 4; ++j) {
            int32_t scaled = di_scale_raw_dais(ddh_dais_proximity[i][j]);
            int32_t last = di_treated_z[i][j];
            di_treated_z[i][j] = (last * di_detection_filter_k +
                scaled * di_detection_filter_one_minus_k) >> 8;
            
            di_detect[i][j] = scaled < di_detection_threshold;
        }
    }
    
    for(size_t i = 0; i < 2; ++i) {
        int32_t motion_x_0 =
            (di_motion_detect(i * 2 + 0, 0, i * 2 + 0, 1) +
            di_motion_detect(i * 2 + 1, 0, i * 2 + 1, 1)) << 15;
        int32_t motion_x_1 =
            (di_motion_detect(i * 2 + 0, 1, i * 2 + 0, 2) +
            di_motion_detect(i * 2 + 1, 1, i * 2 + 1, 2)) << 15;
        int32_t motion_x_2 =
            (di_motion_detect(i * 2 + 0, 2, i * 2 + 0, 3) +
            di_motion_detect(i * 2 + 1, 2, i * 2 + 1, 3)) << 15;
        
        di_raw_motion_quadrants[i][0].x =
            (motion_x_0 * 2 + motion_x_1) / 3;
        di_raw_motion_quadrants[i][1].x =
            (motion_x_1 + motion_x_2 * 2) / 3;
    }
    
    for(size_t i = 0; i < 2; ++i) {
        int32_t motion_y_0 =
            (di_motion_detect(0, i * 2 + 0, 1, i * 2 + 0) +
            di_motion_detect(0, i * 2 + 1, 1, i * 2 + 1)) << 15;
        int32_t motion_y_1 =
            (di_motion_detect(1, i * 2 + 0, 2, i * 2 + 0) +
            di_motion_detect(1, i * 2 + 1, 2, i * 2 + 1)) << 15;
        int32_t motion_y_2 =
            (di_motion_detect(2, i * 2 + 0, 3, i * 2 + 0) +
            di_motion_detect(2, i * 2 + 1, 3, i * 2 + 1)) << 15;
        
        di_raw_motion_quadrants[0][i].y =
            (motion_y_0 * 2 + motion_y_1) / 3;
        di_raw_motion_quadrants[1][i].y =
            (motion_y_1 + motion_y_2 * 2) / 3;
    }
    
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            int32_t delta_z = 0;
            
            for(size_t k = 0; k < 2; ++k) {
                for(size_t l = 0; l < 2; ++l) {
                    size_t r = i * 2 + k;
                    size_t c = j * 2 + l;
                    
                    if(di_detect[r][c] && di_last_detect[r][c]) {
                        delta_z += di_treated_z[r][c] -
                          di_last_treated_z[r][c];
                    }
                }
            }
            di_raw_motion_quadrants[i][j].z = (delta_z * 4) / 4;
        }
    }
    
    di_process_inertia(delta_time);
}


int32_t di_scale_raw_dais(int32_t raw)
{
    if(raw >= 768) {
        return 0;
    }
    else if(raw < 512) {
        return 65536;
    }
    else {
        int32_t offset_raw = (raw - 512) * 2;
        return 65536 - ((offset_raw << 7) | (offset_raw >> 2));
    }
}


int32_t di_motion_detect(size_t r0, size_t c0, size_t r1, size_t c1)
{
    size_t index =
        (di_detect[r0][c0] ? 1 : 0) |
        (di_detect[r1][c1] ? 2 : 0) |
        (di_last_detect[r0][c0] ? 4 : 0) |
        (di_last_detect[r1][c1] ? 8 : 0);
    return di_motion_lookup[index];
}


void di_process_inertia(fix16_t delta_time)
{
    vector3_t a = vector3_make(0, 0, 0);
    fix16_t r_a = 0;
    
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            vector3_t motion = di_raw_motion_quadrants[i][j];
            fix16_t q_a =
                vector3_dot(motion, di_impulse_pos[i][j]);
            fix16_t q_r_a =
                vector3_dot(motion, di_impulse_pos_perp[i][j]);
            
            a = vector3_add(a, vector3_scale(di_impulse_pos[i][j], q_a));
            a.z += motion.z;
            r_a += q_r_a * 2;
        }
    }
    
    di_translation = vector3_add(di_translation, vector3_add(
        vector3_scale(di_translation_v, delta_time),
        vector3_scale(a, fix16_sq(delta_time) / 2)));
    di_flat_rotation += fix16_mul(di_flat_rotation_v, delta_time) +
        fix16_mul(r_a, fix16_sq(delta_time) / 2);
    
    di_translation_v = vector3_add(di_translation_v,
        vector3_scale(a, delta_time));
    di_flat_rotation_v += fix16_mul(r_a, delta_time);
    
    // pretty sure this is mathematically off but I don't want to integrate
    {
        fix16_t f_a = fix16_mul(fix16_from_float(0.05f), delta_time);
        fix16_t f_r_a = fix16_mul(fix16_from_float(0.025f), delta_time);
        fix16_t v_mag = fix16_sqrt(vector3_lengthsq(di_translation_v));
        
        if(v_mag <= f_a) {
            di_translation_v = vector3_make(0, 0, 0);
        }
        else {
            vector3_t dir = vector3_normalize(di_translation_v);
            vector3_t delta_v = vector3_scale(dir, f_a);
            di_translation_v = vector3_sub(di_translation_v, delta_v);
            assert(fix16_sqrt(vector3_lengthsq(di_translation_v)) <= v_mag);
        }
        
        if(abs(di_flat_rotation_v) < f_r_a) {
            di_flat_rotation_v = 0;
        }
        else {
            if(di_flat_rotation_v > 0) {
                di_flat_rotation_v -= f_r_a;
            }
            else {
                di_flat_rotation_v += f_r_a;
            }
        }
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
    */
    ddh_log("v: %6.4f,%6.4f / %6.4f\n", fix16_to_float(di_translation_v.x), fix16_to_float(di_translation_v.y), fix16_to_float(di_flat_rotation_v));
}

