#ifndef DODECAHALL_H_INCLUDED
#define DODECAHALL_H_INCLUDED

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

typedef struct {
    uint8_t r, g, b;
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
extern uint8_t const ddh_center_dodecahedron;
extern uint8_t const ddh_group_dodecahedrons[DDH_TOTAL_GROUPS][
    DDH_DODECAHEDRONS_PER_GROUP];
extern uint8_t const ddh_group_adjacencies[DDH_TOTAL_GROUPS][2];
extern uint8_t const ddh_dodecahedron_adjacencies[DDH_TOTAL_DODECAHEDRONS];
extern placement_t const ddh_structure_placements[DDH_TOTAL_DODECAHEDRONS];
extern vertex_t const ddh_structure_vertex_coords[DDH_TOTAL_DODECAHEDRONS][
    DDH_VERTICES_PER_DODECAHEDRON];

extern uint8_t const ddh_light_dodecahedron[DDH_TOTAL_VERTICES];
extern uint8_t const ddh_light_vertex[DDH_TOTAL_VERTICES];
extern uint8_t const ddh_light_faces[DDH_TOTAL_VERTICES][3];
extern uint8_t const ddh_light_group[DDH_TOTAL_VERTICES];

extern color_t ddh_frame[DDH_TOTAL_VERTICES];

void dodecahall_init(void);
void dodecahall_process(int32_t ns);

#endif // DODECAHALL_H_INCLUDED
