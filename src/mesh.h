#ifndef MESH
#define MESH

#include <stdlib.h>
#include <stdio.h>
#include "vector.h"
#include "triangle.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAX_TRIANGLES_TO_RENDER 1028
extern Triangle* triangles_to_render;
extern int triangle_count;

extern vec3 cube_vertices[8];

typedef struct {
    int vertices_count;
    int face_count;
    vec3 *vertices;
    vec3 rotation;
    Face *faces;
} Mesh;

extern Mesh mesh;

char skip_line(FILE *f);
void load_cube_mesh_data();
void load_obj_file(char *filename, Mesh *mesh);
#endif
