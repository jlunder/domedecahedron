#include "effect_util.h"


fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq, fix16_t phase)
{
    fix16_t dist = fix16_sqrt(
        fix16_sq(a.x - b.x) +
        fix16_sq(a.y - b.y) +
        fix16_sq(a.z - b.z));
    
    return fix16_sin(fix16_mul(dist, freq) + phase);
}

