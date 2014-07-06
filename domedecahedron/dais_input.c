#include "dais_input.h"


static int32_t const di_detection_threshold = 65536;

int32_t di_raw_dais_history[4][3];
size_t di_raw_dais_cur = 0;
int32_t di_treated_z[3];
bool di_last_detect[3];
bool di_detect[3];


int32_t di_scale_raw_dais(int32_t raw);


void di_initialize(void)
{
}

void di_process(void)
{
    memcpy(di_last_detect, di_detect, sizeof di_last_detect);
    memcpy(di_raw_dais_history[di_raw_dais_cur], ddh_dais_proximity,
        sizeof di_raw_dais_history[0]);
    di_raw_dais_cur = (di_raw_dais_cur + 1) % LENGTHOF(di_raw_dais_history);
    
    for(size_t i = 0; i < 3; ++i) {
        int32_t sum = 0;
        
        for(size_t j = 0; j < LENGTHOF(di_raw_dais_history); ++j) {
            sum += di_raw_dais_history[j][i];
        }
        di_treated_z[i] = di_scale_raw_dais(sum);
        di_detect[i] = di_treated_z[i] < di_detection_threshold;
    }
}

int32_t di_scale_raw_dais(int32_t raw)
{
    if(raw >= 768 * (int32_t)LENGTHOF(di_raw_dais_history)) {
        return 0;
    }
    else if(raw < 256 * (int32_t)LENGTHOF(di_raw_dais_history)) {
        return 65536;
    }
    else {
        int32_t offset_raw = raw - 256 * LENGTHOF(di_raw_dais_history);
        return 65536 -
            offset_raw * 65535 / (512 * LENGTHOF(di_raw_dais_history) - 1);
    }
}

