#include "effect_util.h"


fix16_t eu_sin_dist3(vector3_t a, vector3_t b, fix16_t freq, fix16_t phase)
{
    fix16_t dist = fix16_sqrt(
        fix16_sq(a.x - b.x) +
        fix16_sq(a.y - b.y) +
        fix16_sq(a.z - b.z));
    
    return fix16_sin(fix16_mul(dist, freq) + phase);
}


color_t eu_rgb_from_hsv(color_t hsv_color)
{
    return color_make(0, 0, 0);
}


color_t eu_hsv_from_rgb(color_t rgb_color)
{
    return color_make(0, 0, 0);
}


