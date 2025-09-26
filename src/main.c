#include <stdio.h>
#include "platform.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define WIDTH 1280
#define HEIGHT 720
#define FPS 60
static float MS_PER_FRAME = (1000.0 / FPS);

Triangle triangles_to_render[N_CUBE_FACES];

const int n_points = 9 * 9 * 9;
vec3 points[n_points];
vec2 projected_points[n_points];
vec3 cube_rotation = { 0., 0., 0. };

const float fov_factor = 840.0;
vec3 camera_position = { 0., 0., -10. };

vec2 project(vec3 v3) {
    vec2 projected;
    projected.x = (fov_factor * v3.x) / v3.z;
    projected.y = (fov_factor * v3.y) / v3.z;
    return projected;
}

void setup() {

	int point_index = 0;
	for(float x = -1.; x <= 1.; x += 0.25) {
		for(float y = -1.; y <= 1.; y += 0.25) {
			for(float z = -1.; z <= 1.; z += 0.25) {
				vec3 v = { .x = x, .y = y, .z = z};
				points[point_index] = v;
				point_index++;
			}
		}
	}
}

void update() {
	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	// cube_rotation.z += 0.01;
	// for(int i = 0; i < n_points; i++) {
	// 	vec3 point = points[i];

	// 	vec3 transformed_point = vec3_rotate_y(point, cube_rotation.y);
	// 	transformed_point = vec3_rotate_x(transformed_point, cube_rotation.x);
	// 	transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

	// 	transformed_point.z -= camera_position.z;
	// 	projected_points[i] = project(transformed_point);
	// 	projected_points[i].x = (window_width / 2.0) + projected_points[i].x;
	// 	projected_points[i].y = (window_height / 2.0) + projected_points[i].y;
	// }

	for(int i = 0; i < N_CUBE_FACES; i++) {
		Face face = cube_faces[i];
		vec3 face_vertices[3];
		face_vertices[0] = cube_vertices[face.a - 1];
		face_vertices[1] = cube_vertices[face.b - 1];
		face_vertices[2] = cube_vertices[face.c - 1];

		Triangle triangle;

		for(int j = 0; j < 3; j++) {
			vec3 transformed_point = face_vertices[j];	

			transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
			transformed_point = vec3_rotate_x(transformed_point, cube_rotation.x);
			transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

			transformed_point.z -= camera_position.z;

			vec2 projected_point = project(transformed_point);
			projected_point.x = (window_width / 2.0) + projected_point.x;
			projected_point.y = (window_height / 2.0) + projected_point.y;

			triangle.points[j] = projected_point;
		}

		triangles_to_render[i] = triangle;
	}
}

void render() {
	draw_grid();

	// for(int i = 0; i < n_points; i++) {
	// 	vec2 point = projected_points[i];
	// 	draw_rect(0xFFFF0000, point.y, point.x, 4, 4);
	// }

	for(int i = 0; i < N_CUBE_FACES; i++) {
		Triangle triangle = triangles_to_render[i];
		draw_rect(0xFFFF0000, triangle.points[0].y, triangle.points[0].x, 4, 4);
		draw_rect(0xFFFF0000, triangle.points[1].y, triangle.points[1].x, 4, 4);
		draw_rect(0xFFFF0000, triangle.points[2].y, triangle.points[2].x, 4, 4);
	}

	render_color_buffer();
	render_present();
}

int main() {

	init_window(WIDTH, HEIGHT, 1);

	setup();

    int last_ms = get_ticks_ms();
	
    while(running == 1) {
		process_events();
		update();
		render();

		// enforce fixed frame rate
		int time_to_wait = MS_PER_FRAME - (get_ticks_ms() - last_ms);
		if(time_to_wait > 0) {
			wait_ticks_ms(time_to_wait);
		}
		last_ms = get_ticks_ms();
    }

	destroy_window();
}
