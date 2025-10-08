#include "mesh.h"
#include "triangle.h"
#include "array.h"
#include <stdio.h>

Triangle *triangles_to_render = NULL;
Triangle *triangles_to_render_scratch = NULL;
int triangle_count = 0;

vec3 cube_vertices[8] = {
    { .x = -1,  .y = -1,  .z = -1},
    { .x = -1,  .y = 1,   .z = -1},
    { .x = 1,   .y = 1,   .z = -1},
    { .x = 1,   .y = -1,  .z = -1},
    { .x = 1,   .y = 1,   .z = 1},
    { .x = 1,   .y = -1,  .z = 1},
    { .x = -1,  .y = 1,   .z = 1},
    { .x = -1,  .y = -1,  .z = 1}
};

Mesh mesh = {
    .face_count = 0,
    .faces = NULL,

    .vertices_count = 0,
    .vertices = NULL,

    .scale 		 = { .1, .1, .1 },
    .translation = { .0, .0, .0 },
    .rotation 	 = { .0, .0, .0 }
};

char skip_line(FILE *f) {
	char next = getc(f);
	while(next != EOF && next != '\n')
		next = getc(f);
	return next;
}

void load_cube_mesh_data() {
    mesh.face_count = ARRAY_SIZE(cube_faces);
    mesh.vertices_count = ARRAY_SIZE(cube_vertices);

    mesh.vertices = (vec3 *) malloc(mesh.vertices_count * sizeof(mesh.vertices[0]));
    mesh.faces = (Face *) malloc(mesh.face_count * sizeof(mesh.faces[0]));

    for(int i = 0; i < mesh.vertices_count; i++) {
        mesh.vertices[i] = cube_vertices[i];
    }

    for(int i = 0; i < mesh.face_count; i++) {
        mesh.faces[i] = cube_faces[i];
    }

    mesh.rotation = (vec3) { .0, .0, .0 };
}

void load_obj_file(char *filename, Mesh *mesh) {

	FILE *obj_file = fopen(filename, "r");
	if(!obj_file) {
		printf("Couldn't open file: \'%s\'\n", filename);
		return;
	}
	
	mesh->vertices = NULL;
	mesh->faces = NULL;
	mesh->vertices_count = 0;
	mesh->face_count = 0;

	char type;
	float vx, vy, vz;
	int f1, f2, f3;

	while((type = getc(obj_file)) != EOF) {
		if(type == 'v') {
			fscanf(obj_file, " %f %f %f\n", &vx, &vy, &vz);
			printf("%c %f %f %f\n", type, vx, vy, vz);
			vec3 vertex;
			vertex.x = vx;
			vertex.y = - vy;
			vertex.z = vz;
			array_push(mesh->vertices, vertex);
			mesh->vertices_count++;
		}
		else if(type == 'f') {
			fscanf(obj_file, " %d %d %d\n", &f1, &f2, &f3);
			printf("%c %d %d %d\n", type, f1, f2, f3);
			Face face;
			face.a = f1;
			face.b = f2;
			face.c = f3;
			array_push(mesh->faces, face);
			mesh->face_count++;
		}

		// skip the line if we don't recognise the first character
		else {
			skip_line(obj_file);
		}
	}
		
	// printf("loaded %d vertices\n", array_length(mesh->vertices));
	// printf("loaded %d faces\n", array_length(mesh->faces));
	fclose(obj_file);
}
