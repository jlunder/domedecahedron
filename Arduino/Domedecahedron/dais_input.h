#ifndef DAIS_INPUT_H_INCLUDED
#define DAIS_INPUT_H_INCLUDED

#include "domedecahedron.h"

extern vector3_t di_motion_quadrants[2][2];

extern vector3_t di_rotation;
extern fix16_t di_flat_rotation;
extern fix16_t di_flat_rotation_v;
extern vector3_t di_translation;
extern vector3_t di_translation_v;

extern vector3_t di_raw_motion_quadrants[2][2];

extern fix16_t di_last_treated_z[4][4];
extern fix16_t di_treated_z[4][4];
extern bool di_last_detect[4][4];
extern bool di_detect[4][4];

void di_initialize(void);
void di_process(fix16_t delta_time);


#endif // DAIS_INPUT_H_INCLUDED
