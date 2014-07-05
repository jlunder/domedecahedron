#include "dais_input.h"


static int32_t const di_detection_filter_k = (int32_t)(256 * 0.9);
static int32_t const di_detection_filter_one_minus_k =
  256 - di_detection_filter_k;
static int32_t const di_detection_threshold = 65536;

int32_t di_last_treated_z[3];
int32_t di_treated_z[3];
bool di_last_detect[3];
bool di_detect[3];


int32_t di_scale_raw_dais(int32_t raw);


void di_initialize(void)
{
    memset(di_last_detect, 0, sizeof di_last_detect);
}

void di_process(void)
{
    memcpy(di_last_detect, di_detect, sizeof di_last_detect);
    memcpy(di_last_treated_z, di_treated_z, sizeof di_last_treated_z);
    
    for(size_t i = 0; i < 3; ++i) {
        int32_t scaled = di_scale_raw_dais(ddh_dais_proximity[i]);
        int32_t last = di_treated_z[i];
        di_treated_z[i] = (last * di_detection_filter_k +
            scaled * di_detection_filter_one_minus_k) >> 8;
        
        di_detect[i] = scaled < di_detection_threshold;
    }
}

int32_t di_scale_raw_dais(int32_t raw)
{
    if(raw >= 768) {
        return 0;
    }
    else if(raw < 256) {
        return 65536;
    }
    else {
        int32_t offset_raw = raw - 256;
        return 65536 - ((offset_raw << 7) | (offset_raw >> 2));
    }
}

