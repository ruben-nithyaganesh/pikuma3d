#include <stdio.h>
#include "platform.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

static float MS_PER_FRAME = (1000.0 / FPS);

const float fov_factor = 840.0;
vec3 camera_position = { 0., 0., -90. };

vec2 project(vec3 v3) {
    vec2 projected;
    projected.x = (fov_factor * v3.x) / v3.z;
    projected.y = (fov_factor * v3.y) / v3.z;
    return projected;
}

void setup() {
	// load_cube_mesh_data();
	load_obj_file("assets/teddy.obj", &mesh);
	triangles_to_render = (Triangle *) malloc(mesh.face_count * sizeof(Triangle));
	triangle_count = 0;
	
	flags = (flags | F_ROTATE);
	flags = (flags | F_BACK_FACE_CULLING);
}

void update() {

	if(flags & F_ROTATE) {
		mesh.rotation.x += 0.004;
		mesh.rotation.y -= 0.005;
		// mesh.rotation.z += 0.006;
	}

	triangle_count = 0;
	for(int i = 0; i < mesh.face_count; i++) {
		Face face = mesh.faces[i];
		vec3 face_vertices[3];
		face_vertices[0] = mesh.vertices[face.a - 1];
		face_vertices[1] = mesh.vertices[face.b - 1];
		face_vertices[2] = mesh.vertices[face.c - 1];
		
		// transform each vertex of the current face according to the current mesh rotation
		vec3 transformed_face_vertices[3];
		for(int j = 0; j < 3; j++) {
			vec3 transformed_point = face_vertices[j];	
			transformed_point = vec3_rotate_y(transformed_point, mesh.rotation.y);
			transformed_point = vec3_rotate_x(transformed_point, mesh.rotation.x);
			transformed_point = vec3_rotate_z(transformed_point, mesh.rotation.z);
			transformed_point.z -= camera_position.z;

			transformed_face_vertices[j] = transformed_point;
		}
		
		// default dot_prod > 0.0, i.e we draw the triangle
		float dot_prod = 1.0;

		// if back face culling is enabled, compute actual dot prod
		// of face normal to camera position vector
		if(flags & F_BACK_FACE_CULLING) {
			vec3 a = transformed_face_vertices[1];
			vec3 b = transformed_face_vertices[0];
			vec3 c = transformed_face_vertices[2];

			vec3 ab = vec3_sub(b, a);
			vec3 ac = vec3_sub(c, a);
			vec3 normal = vec3_cross_prod(ab, ac);
			vec3 camera_ray = vec3_sub(camera_position, a);

			dot_prod = vec3_dot(normal, camera_ray);
		}
		
		
		int should_render_face = (dot_prod > 0.0);
		// if we should render the current face, project face vertices
		// into a triangle to be rendered
		if(should_render_face) {
			Triangle triangle;
			for(int j = 0; j < 3; j++) {
				vec2 projected_point = project(transformed_face_vertices[j]);
				projected_point.x = (window_width / 2.0) + projected_point.x;
				projected_point.y = (window_height / 2.0) + projected_point.y;
				triangle.points[j] = projected_point;
			}
			triangles_to_render[triangle_count++] = triangle;
		}
	}
}

void render() {
	draw_grid();
	
	for(int i = 0; i < triangle_count; i++) {
		Triangle triangle = triangles_to_render[i];

		fill_triangle(
			0xFF999999,
			triangle.points[0].x, triangle.points[0].y,
			triangle.points[1].x, triangle.points[1].y,
			triangle.points[2].x, triangle.points[2].y
		);

		if(flags & F_DRAW_VERTICES) {
			uint32_t rect_col = 0xFFFF0000;
			draw_rect(rect_col, triangle.points[0].y - 2, triangle.points[0].x - 2, 5, 5);
			draw_rect(rect_col, triangle.points[1].y - 2, triangle.points[1].x - 2, 5, 5);
			draw_rect(rect_col, triangle.points[2].y - 2, triangle.points[2].x - 2, 5, 5);
			draw_triangle(
				0xFF222222,
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y
			);
		}
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
