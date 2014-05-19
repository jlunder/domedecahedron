#ifndef DOMEDECAHEDRON_H_INCLUDED
#define DOMEDECAHEDRON_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>


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

typedef union {
    struct {
        uint8_t r, g, b;
    };
    struct {
        uint32_t color;
    };
} color_t;

typedef struct {
    float x, y, z, w;
} quaternion_t;

typedef struct {
    vertex_t position;
    float size;
    quaternion_t orientation;
} placement_t;


extern vertex_t const ddh_dodecahedron_vertex_coords[
    DDH_VERTICES_PER_DODECAHEDRON];
extern vertex_t const ddh_vertex_coords[DDH_TOTAL_VERTICES];

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


void domedecahedron_init(void);
void domedecahedron_process(uint64_t delta_ns);

uint64_t ddh_ns_since(uint64_t total_ns);
uint32_t ddh_ms_since(uint64_t total_ns);
uint32_t ddh_frames_since(uint32_t frame);


#endif // DOMEDECAHEDRON_H_INCLUDED
