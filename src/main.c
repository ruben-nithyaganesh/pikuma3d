#include <stdio.h>
#include "platform.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"

#define WIDTH 1280
#define HEIGHT 720
// #define WIDTH 500
// #define HEIGHT 500
#define FPS 60

static float MS_PER_FRAME = (1000.0 / FPS);
static float F_ONE_THIRD = (1.0f / 3.0f);

const float fov_factor = 840.0;
vec3 camera_position = { 0., 0., -10.};
vec3 camera_rotation = { 0., 0., 0. };

vec2 project(vec3 v3) {
    vec2 projected;
    projected.x = (v3.z == 0.0) ? 0.0 : (fov_factor * v3.x) / v3.z;
    projected.y = (v3.z == 0.0) ? 0.0 : (fov_factor * v3.y) / v3.z;
    return projected;
}

void setup() {

	// load_cube_mesh_data();
	load_obj_file("assets/teddy.obj", &mesh);
	triangles_to_render = (Triangle *) malloc(mesh.face_count * sizeof(Triangle));
	triangles_to_render_scratch = (Triangle *) malloc(mesh.face_count * sizeof(Triangle));
		
	mesh.translation.z -= camera_position.z;
	triangle_count = 0;
	
	flags = 0x00000000;
	// flags = (flags | F_ROTATE);
	flags = (flags | F_BACK_FACE_CULLING);
	flags = (flags | F_FILL);
	// flags = (flags | F_DRAW_VERTICES);
	flags = (flags | F_DRAW_LINES);
	flags = (flags | F_SORT_Z_DEPTH);
}

void camera_update() {
	if(controller & C_LEFT) {
		camera_position.x -= 0.05;
	}
	if(controller & C_RIGHT) {
		camera_position.x += 0.05;
	}
	if(controller & C_UP) {
		camera_position.z += 0.05;
	}
	if(controller & C_DOWN) {
		camera_position.z -= 0.05;
	}
	if(controller & C_LOOK_LEFT) {
		camera_rotation.y += 0.05;
	}
	if(controller & C_LOOK_RIGHT) {
		camera_rotation.y -= 0.05;
	}
}
void update() {
	
	camera_update();

	if(flags & F_ROTATE) {
		mesh.rotation.y += 0.008;
		mesh.rotation.z += 0.008;
	}

	triangle_count = 0;

	mat4 scale_matrix = mat4_scale_matrix(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4 translation_matrix = mat4_translation_matrix(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4 x_rotation_matrix = mat4_rotation_matrix_x(mesh.rotation.x);
	mat4 y_rotation_matrix = mat4_rotation_matrix_y(mesh.rotation.y);
	mat4 z_rotation_matrix = mat4_rotation_matrix_z(mesh.rotation.z);
	
	mat4 transform = mat4_identity();
	transform = mat4_mul(scale_matrix, transform);
	transform = mat4_mul(x_rotation_matrix, transform);
	transform = mat4_mul(y_rotation_matrix, transform);
	transform = mat4_mul(z_rotation_matrix, transform);
	transform = mat4_mul(translation_matrix, transform);

	for(int i = 0; i < mesh.face_count; i++) {
		Face face = mesh.faces[i];
		vec3 face_vertices[3];
		face_vertices[0] = mesh.vertices[face.a - 1];
		face_vertices[1] = mesh.vertices[face.b - 1];
		face_vertices[2] = mesh.vertices[face.c - 1];
		
		// transform each vertex of the current face according to the current mesh rotation
		vec3 transformed_face_vertices[3];
		for(int j = 0; j < 3; j++) {
			vec4 transformed_point = vec4_from_vec3(face_vertices[j]);
			// transformed_point = vec3_rotate_y(transformed_point, mesh.rotation.y);
			// transformed_point = vec3_rotate_x(transformed_point, mesh.rotation.x);
			// transformed_point = vec3_rotate_z(transformed_point, mesh.rotation.z);

			// transformed_point = mat4_mul_vec4(x_rotation_matrix, transformed_point);
			// transformed_point = mat4_mul_vec4(y_rotation_matrix, transformed_point);
			// transformed_point = mat4_mul_vec4(z_rotation_matrix, transformed_point);
			// transformed_point = mat4_mul_vec4(scale_matrix, transformed_point);
			// transformed_point = mat4_mul_vec4(translation_matrix, transformed_point);

			transformed_point = mat4_mul_vec4(transform, transformed_point);

			transformed_face_vertices[j] = vec3_from_vec4(transformed_point);
		}
		
		// default dot_prod > 0.0, i.e we draw the triangle
		float dot_prod = 1.0;

		// if back face culling is enabled, compute actual dot prod
		// of face normal to camera position vector
		if(flags & F_BACK_FACE_CULLING) {
			vec3 a = transformed_face_vertices[0];
			vec3 b = transformed_face_vertices[1];
			vec3 c = transformed_face_vertices[2];

			vec3 ab = vec3_sub(b, a);
			vec3 ac = vec3_sub(c, a);
			vec3 normal = vec3_cross_prod(ab, ac);
			vec3 camera_ray = vec3_sub(camera_position, a);

			dot_prod = vec3_dot(normal, camera_ray);
		}
		
		
		int should_render_face = (dot_prod > 0.0);
		for(int i = 0; i < 3; i++) {
			if(transformed_face_vertices[i].z <= (camera_position.z + 5.0)) {
				should_render_face = 0;
			}
		}
		// if we should render the current face, project face vertices
		// into a triangle to be rendered
		if(should_render_face) {
			Triangle triangle;
			triangle.avg_depth = 0.0;
			for(int j = 0; j < 3; j++) {
				vec2 projected_point = project(transformed_face_vertices[j]);
				projected_point.x = (window_width / 2.0) + projected_point.x;
				projected_point.y = (window_height / 2.0) + projected_point.y;
				triangle.points[j] = projected_point;
				
				triangle.avg_depth += transformed_face_vertices[j].z * F_ONE_THIRD;
			}
			triangles_to_render[triangle_count++] = triangle;
		}
	}
}

void render() {
	draw_gradient(0xFF0055FF, 0xFFFFFFFF);

	if(flags & F_SORT_Z_DEPTH) {
		merge_sort_triangles(triangles_to_render, triangles_to_render_scratch, triangle_count);
	}

	for(int i = 0; i < triangle_count; i++) {
		Triangle triangle = triangles_to_render[i];

		if(flags & F_FILL) {
			fill_triangle(
				0xFF999999,
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y
			);
		}

		if(flags & F_DRAW_VERTICES) {
			uint32_t rect_col = 0xFFFF0000;
			draw_rect(rect_col, triangle.points[0].y - 1, triangle.points[0].x - 1, 3, 3);
			draw_rect(rect_col, triangle.points[1].y - 1, triangle.points[1].x - 1, 3, 3);
			draw_rect(rect_col, triangle.points[2].y - 1, triangle.points[2].x - 1, 3, 3);
		}

		if(flags & F_DRAW_LINES) {
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
