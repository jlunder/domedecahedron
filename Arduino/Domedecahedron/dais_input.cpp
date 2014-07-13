#include "dais_input.h"


static int32_t const di_motion_lookup[16] = {
    0, 1, -1, 0, -1, 0, 1, 1, 1, -1, 0, -1, 0, -1, 1, 0,
};
static int32_t const di_detection_filter_k = (int32_t)(256 * 0.9);
static int32_t const di_detection_filter_one_minus_k =
  256 - di_detection_filter_k;
static int32_t const di_detection_threshold = 65536;

vector3_t di_motion_quadrants[2][2];

vector3_t di_rotation;
vector2_t di_flat_rotation;
vector3_t di_translation;

int32_t di_last_treated_z[4][4];
int32_t di_treated_z[4][4];
bool di_last_detect[4][4];
bool di_detect[4][4];

vector3_t di_raw_motion_quadrants[2][2];


int32_t di_scale_raw_dais(int32_t raw);
int32_t di_motion_detect(size_t r0, size_t c0, size_t r1, size_t c1);


void di_initialize(void)
{
    memset(di_last_detect, 0, sizeof di_last_detect);
}


void di_process(void)
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
            di_raw_motion_quadrants[i][j].z = delta_z / 4;
        }
    }
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

