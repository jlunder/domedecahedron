#include "effect.h"

#include "effect_util.h"


void effect_plasma_0_process(void * state, fix16_t delta_time,
    color_t buf[DDH_TOTAL_VERTICES]);

effect_t const effect_plasma_0 = {
    NULL,
    NULL,
    &effect_plasma_0_process,
    NULL,
    NULL,
};

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

