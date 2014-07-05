#ifndef DAIS_INPUT_H_INCLUDED
#define DAIS_INPUT_H_INCLUDED

#include "domedecahedron.h"

extern int32_t di_last_treated_z[3];
extern int32_t di_treated_z[3];
extern bool di_last_detect[3];
extern bool di_detect[3];

void di_initialize(void);
void di_process(void);


#endif // DAIS_INPUT_H_INCLUDED
