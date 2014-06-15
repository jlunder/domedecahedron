#ifndef DOMEDECAHEDRON_H_INCLUDED
#define DOMEDECAHEDRON_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fix16.h"


#define PI_F 3.1415926535897932384626f
#define TWO_PI_F (2.0f * 3.1415926535897932384626f)

#define DDH_FPS 60

#define DDH_VERTICES_PER_DODECAHEDRON 20
#define DDH_FACES_PER_DODECAHEDRON 12
#define DDH_VERTICES_PER_FACE 5
#define DDH_DODECAHEDRONS_PER_GROUP 6
#define DDH_TOTAL_GROUPS 6
#define DDH_TOTAL_DODECAHEDRONS (DDH_DODECAHEDRONS_PER_GROUP * DDH_TOTAL_GROUPS)
#define DDH_TOTAL_VERTICES (DDH_VERTICES_PER_DODECAHEDRON * DDH_TOTAL_DODECAHEDRONS)

typedef struct {
    float x, y, z;
} vertex_t;

// 1.15.16 fixed point
typedef struct {
    fix16_t x;
    fix16_t y;
} vector2_t;

typedef struct {
    fix16_t x;
    fix16_t y;
    fix16_t z;
} vector3_t;

typedef struct {
    fix16_t x;
    fix16_t y;
    fix16_t z;
    fix16_t w;
} vector4_t;

typedef union {
    struct {
        uint8_t r, g, b, x;
    };
    uint32_t color;
} color_t;

typedef struct {
    float x, y, z, w;
} quaternion_t;

typedef struct {
    vertex_t position;
    float size;
    quaternion_t orientation;
} placement_t;


static color_t const color_black =   {{  0,   0,   0, 0}};
static color_t const color_brown =   {{127,  63,   0, 0}};
static color_t const color_red =     {{255,   0,   0, 0}};
static color_t const color_orange =  {{191,  95,   0, 0}};
static color_t const color_yellow =  {{255, 255,   0, 0}};
static color_t const color_green =   {{  0, 255,   0, 0}};
static color_t const color_blue =    {{  0,   0, 255, 0}};
static color_t const color_purple =  {{191,   0, 255, 0}};
static color_t const color_grey =    {{127, 127, 127, 0}};
static color_t const color_white =   {{255, 255, 255, 0}};

static color_t const color_cyan =    {{  0, 255, 255, 0}};
static color_t const color_magenta = {{255,   0, 255, 0}};


static inline color_t color_make(uint8_t r, uint8_t g, uint8_t b) {
    color_t c = {{r, g, b, 0}};
    return c;
}

static inline color_t color_add_sat(color_t x, color_t y)
{
    int32_t r = (int32_t)x.r + (int32_t)y.r;
    int32_t g = (int32_t)x.g + (int32_t)y.g;
    int32_t b = (int32_t)x.b + (int32_t)y.b;
    
    return color_make(r > 255 ? 255 : r, g > 255 ? 255 : g,
        b > 255 ? 255 : b);
}

static inline vector2_t vector2_make(fix16_t x, fix16_t y) {
    vector2_t v = {x, y};
    return v;
}

static inline vector3_t vector3_make(fix16_t x, fix16_t y, fix16_t z) {
    vector3_t v = {x, y, z};
    return v;
}


extern vertex_t const ddh_dodecahedron_vertex_coords[
    DDH_VERTICES_PER_DODECAHEDRON];
extern vertex_t const ddh_vertex_coords[DDH_TOTAL_VERTICES];
extern vector3_t ddh_vertex_coords_fix[DDH_TOTAL_VERTICES];

extern float const ddh_dodecahedron_transforms[
    DDH_DODECAHEDRONS_PER_GROUP][16];
extern float const ddh_group_transforms[DDH_TOTAL_GROUPS][16];

extern uint8_t const ddh_dodecahedron_vertex_adjacency[
    DDH_VERTICES_PER_DODECAHEDRON][3];
extern uint8_t const ddh_dodecahedron_vertex_opposition[
    DDH_VERTICES_PER_DODECAHEDRON];
extern uint8_t const ddh_dodecahedron_face_vertices[
    DDH_FACES_PER_DODECAHEDRON][DDH_VERTICES_PER_FACE];
extern uint8_t const ddh_dodecahedron_face_adjacency[
    DDH_FACES_PER_DODECAHEDRON][5];
extern uint8_t const ddh_dodecahedron_face_opposition[
    DDH_FACES_PER_DODECAHEDRON];
static uint8_t const ddh_center_dodecahedron = 0;
extern uint8_t const ddh_group_dodecahedrons[DDH_TOTAL_GROUPS][
    DDH_DODECAHEDRONS_PER_GROUP];
extern uint8_t const ddh_dodecahedron_adjacencies[DDH_TOTAL_DODECAHEDRONS];

extern uint8_t const ddh_light_dodecahedron[DDH_TOTAL_VERTICES];
extern uint8_t const ddh_light_vertex[DDH_TOTAL_VERTICES];
extern uint8_t const ddh_light_faces[DDH_TOTAL_VERTICES][3];
extern uint8_t const ddh_light_group[DDH_TOTAL_VERTICES];

extern color_t ddh_frame_buffer[DDH_TOTAL_VERTICES];

extern uint64_t ddh_total_ns;
extern uint32_t ddh_total_frames;

#define DDH_MODE_RUN                           0
#define DDH_MODE_CONFIGURE                     1
#define DDH_MODE_DEBUG_LOCATE                  5
#define DDH_MODE_DEBUG_SWEEP                   6
#define DDH_MODE_DEBUG_COLOR                   7
#define DDH_MODE_DEBUG_LOG                     9

#define DDH_SUBMODE_DEBUG_LOCATE_VERTICES      0
#define DDH_SUBMODE_DEBUG_LOCATE_DODECAHEDRONS 1
#define DDH_SUBMODE_DEBUG_LOCATE_FACES         2
#define DDH_SUBMODE_DEBUG_LOCATE_GROUPS        3

#define DDH_SUBMODE_DEBUG_SWEEP_ALL            0
#define DDH_SUBMODE_DEBUG_SWEEP_X              1
#define DDH_SUBMODE_DEBUG_SWEEP_Y              2
#define DDH_SUBMODE_DEBUG_SWEEP_Z              3
#define DDH_SUBMODE_DEBUG_SWEEP_STEP_X         4
#define DDH_SUBMODE_DEBUG_SWEEP_STEP_Y         5
#define DDH_SUBMODE_DEBUG_SWEEP_STEP_Z         6

#define DDH_SUBMODE_DEBUG_COLOR_WHITE          0
#define DDH_SUBMODE_DEBUG_COLOR_RED            1
#define DDH_SUBMODE_DEBUG_COLOR_GREEN          2
#define DDH_SUBMODE_DEBUG_COLOR_BLUE           3
#define DDH_SUBMODE_DEBUG_COLOR_STEP_WHITE     4
#define DDH_SUBMODE_DEBUG_COLOR_STEP_RED       5
#define DDH_SUBMODE_DEBUG_COLOR_STEP_GREEN     6
#define DDH_SUBMODE_DEBUG_COLOR_STEP_BLUE      7
#define DDH_SUBMODE_DEBUG_COLOR_GRADIENT       8

extern uint8_t ddh_mode;
extern uint8_t ddh_submode;
extern bool ddh_button_a;
extern bool ddh_button_a_edge;
extern bool ddh_button_b;
extern bool ddh_button_b_edge;

extern uint32_t ddh_dais_proximity[4][4];

void ddh_initialize(void);
void ddh_process(uint64_t delta_ns);

uint64_t ddh_ns_since(uint64_t total_ns);
uint32_t ddh_ms_since(uint64_t total_ns);
uint32_t ddh_frames_since(uint32_t frame);


#endif // DOMEDECAHEDRON_H_INCLUDED
