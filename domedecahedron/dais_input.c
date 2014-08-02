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

fix16_t di_z;
fix16_t di_last_z;

vector3_t di_raw_motion_quadrants[2][2];


int32_t di_scale_raw_dais(int32_t raw);
int32_t di_motion_detect(size_t r0, size_t c0, size_t r1, size_t c1);
void di_process_delta_z(fix16_t delta_time);
void di_process_inertia(fix16_t delta_time);
fix16_t di_modulo(fix16_t x, fix16_t y);
fix16_t di_clamp(fix16_t x, fix16_t y);


void di_initialize(void)
{
    memset(di_last_detect, 0, sizeof di_last_detect);
    
    di_rotation = vector3_make(0, 0, 0);
    di_flat_rotation = 0;
    di_flat_rotation_v = 0;
    di_translation = vector3_make(0, 0, 0);
    di_translation_v = vector3_make(0, 0, 0);
    
    di_z = fix16_one;
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
            //ddh_log("%1d", di_detect[i][j]);
        }
        //ddh_log("\n");
    }
    
    for(size_t i = 0; i < 2; ++i) {
        fix16_t motion_x_0 =
            (di_motion_detect(i * 2 + 0, 0, i * 2 + 0, 1) +
            di_motion_detect(i * 2 + 1, 0, i * 2 + 1, 1)) * fix16_one;
        fix16_t motion_x_1 =
            (di_motion_detect(i * 2 + 0, 1, i * 2 + 0, 2) +
            di_motion_detect(i * 2 + 1, 1, i * 2 + 1, 2)) * fix16_one;
        fix16_t motion_x_2 =
            (di_motion_detect(i * 2 + 0, 2, i * 2 + 0, 3) +
            di_motion_detect(i * 2 + 1, 2, i * 2 + 1, 3)) * fix16_one;
        
        di_raw_motion_quadrants[1 - i][0].x =
            (motion_x_0 * 2 + motion_x_1) / 3;
        di_raw_motion_quadrants[1 - i][1].x =
            (motion_x_1 + motion_x_2 * 2) / 3;
    }
    
    for(size_t i = 0; i < 2; ++i) {
        fix16_t motion_y_0 =
            (di_motion_detect(0, i * 2 + 0, 1, i * 2 + 0) +
            di_motion_detect(0, i * 2 + 1, 1, i * 2 + 1)) * fix16_one;
        fix16_t motion_y_1 =
            (di_motion_detect(1, i * 2 + 0, 2, i * 2 + 0) +
            di_motion_detect(1, i * 2 + 1, 2, i * 2 + 1)) * fix16_one;
        fix16_t motion_y_2 =
            (di_motion_detect(2, i * 2 + 0, 3, i * 2 + 0) +
            di_motion_detect(2, i * 2 + 1, 3, i * 2 + 1)) * fix16_one;
        
        di_raw_motion_quadrants[1 - 0][i].y =
            (motion_y_0 * 2 + motion_y_1) / 3;
        di_raw_motion_quadrants[1 - 1][i].y =
            (motion_y_1 + motion_y_2 * 2) / 3;
    }
    
    di_process_delta_z(delta_time);
    
    di_process_inertia(delta_time);
}


fix16_t di_scale_raw_dais(int32_t raw)
{
    if(raw >= 768) {
        return 0;
    }
    else if(raw < 512) {
        return fix16_one;
    }
    else {
        int32_t offset_raw = (raw - 512) * 2;
        return fix16_one - (fix16_one * offset_raw) / 511;
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


void di_process_delta_z(fix16_t delta_time)
{
    fix16_t delta_z;
    
    di_last_z = di_z;
    
    di_z = fix16_one;
    for(size_t i = 0; i < 4; ++i) {
        for(size_t j = 0; j < 4; ++j) {
            if(di_treated_z[i][j] < di_z) {
                di_z = di_treated_z[i][j];
            }
        }
    }
    //ddh_log("z: %6.4f", fix16_to_float(di_z));
    
    if(abs(di_z - di_last_z) > fix16_from_float(0.25f)) {
        //ddh_log(" z warp!");
        delta_z = 0;
    }
    else {
        delta_z = fix16_div(di_z - di_last_z, delta_time * 100);
    }
    //ddh_log(" dz: %7.4f\n", fix16_to_float(delta_z));
    
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            di_raw_motion_quadrants[i][j].z = delta_z;
        }
    }
}


void di_process_inertia(fix16_t delta_time)
{
    vector3_t j = vector3_make(0, 0, 0);
    fix16_t r_j = 0;
    
    for(size_t k = 0; k < 2; ++k) {
        for(size_t l = 0; l < 2; ++l) {
            vector3_t motion = di_raw_motion_quadrants[k][l];
            fix16_t q_j =
                vector3_dot(motion, di_impulse_pos[k][l]) * 5;
            fix16_t q_r_j =
                vector3_dot(motion, di_impulse_pos_perp[k][l]);
            
            j = vector3_add(j, vector3_scale(di_impulse_pos[k][l], q_j));
            j.z += motion.z;
            r_j += q_r_j * 6;
        }
    }
    
    di_translation_v = vector3_add(di_translation_v,
        vector3_scale(j, fix16_one / 50));
    di_flat_rotation_v += r_j / 50;
    
    di_translation = vector3_add(di_translation,
        vector3_scale(di_translation_v, delta_time));
    di_flat_rotation += fix16_mul(di_flat_rotation_v, delta_time);
    
    // pretty sure this is mathematically off but I don't want to integrate
    {
        fix16_t f_a = fix16_mul(fix16_from_float(0.5f), delta_time);
        fix16_t f_r_a = fix16_mul(fix16_from_float(1.0f), delta_time);
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
    
    di_translation.x = di_modulo(di_translation.x, fix16_one);
    di_translation.y = di_modulo(di_translation.y, fix16_one);
    di_translation.z = di_modulo(di_translation.z, fix16_one);
    di_translation_v.x = di_clamp(di_translation_v.x, fix16_from_float(10.0f));
    di_translation_v.y = di_clamp(di_translation_v.y, fix16_from_float(10.0f));
    di_translation_v.z = di_clamp(di_translation_v.z, fix16_from_float(10.0f));
    di_flat_rotation = di_modulo(di_flat_rotation, fix16_pi);
    di_flat_rotation_v = di_clamp(di_flat_rotation_v, fix16_two_pi);
    
    /*
    ddh_log("motion:\n");
    for(size_t i = 0; i < 2; ++i) {
        for(size_t j = 0; j < 2; ++j) {
            ddh_log("%s ",
                di_raw_motion_quadrants[i][j].x < fix16_from_float(-0.10f) ? " <" :
                di_raw_motion_quadrants[i][j].x < fix16_from_float( 0.10f) ? "  " :
                                                                             " >");
            ddh_log("%s ",
                di_raw_motion_quadrants[i][j].y < fix16_from_float(-0.10f) ? "." :
                di_raw_motion_quadrants[i][j].y < fix16_from_float( 0.10f) ? " " :
                                                                             "^");
        }
        ddh_log("\n");
    }
    ddh_log("\n");
    */
    //ddh_log("v: %7.4f,%7.4f,%7.4f / %6.4f\n", fix16_to_float(di_translation_v.x), fix16_to_float(di_translation_v.y), fix16_to_float(di_translation_v.z), fix16_to_float(di_flat_rotation_v));
}


fix16_t di_modulo(fix16_t x, fix16_t y)
{
    if(x >= y) {
        return (x + y) % (y * 2) - y;
    }
    else if(x < -y) {
        return (x - y + 1) % (y * 2) + y -1;
    }
    else {
        return x;
    }
}


fix16_t di_clamp(fix16_t x, fix16_t y)
{
    if(x > y) {
        return y;
    }
    else if(x < -y) {
        return -y;
    }
    else {
        return x;
    }
}

