#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "platform.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "lighting.h"
#include "array.h"

#define WIDTH 1280
#define HEIGHT 720
// #define WIDTH 500
// #define HEIGHT 500
#define FPS 60

static float MS_PER_FRAME = (1000.0 / FPS);

const float fov_factor = 840.0;
vec3 camera_position = { 0., 0., -10.};
vec3 camera_rotation = { 0., 0., 0. };

Lighting lighting;
mat4 projection_matrix;

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
	

	// default flags
	{
		flags = 0x00000000;
		flags = (flags | F_ROTATE);
		flags = (flags | F_BACK_FACE_CULLING);
		flags = (flags | F_FILL);
		flags = (flags | F_SORT_Z_DEPTH);
	}

	float fov = M_PI / 3.0; // 60deg
	float aspect_ratio = ((float)window_height / (float)window_width);
	float znear = 5.0;
	float zfar = 80.0;


	projection_matrix = mat4_projection_matrix(
		fov,
		aspect_ratio,
		znear,
		zfar
	);
	
	vec3 lighting_dir;
	lighting_dir.x = 1.0;
	lighting_dir.y = -1.0;
	lighting_dir.z = 1.0;
	lighting.global_illumination_direction = vec3_normalise(lighting_dir);
}

void camera_update() {
	if(controller & C_LEFT) {
		camera_position.x -= 0.05;
		float fov = M_PI / 2.0; // 60deg
		float aspect_ratio = ((float)window_height / (float)window_width);
		float znear = 5.0;
		float zfar = 80.0;
		projection_matrix = mat4_projection_matrix(
			fov,
			aspect_ratio,
			znear,
			zfar
		);
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
		//mesh.rotation.z += 0.008;
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
			transformed_point = mat4_mul_vec4(transform, transformed_point);
			transformed_face_vertices[j] = vec3_from_vec4(transformed_point);
		}
		
		// if back face culling is enabled, compute actual dot prod
		// of face normal to camera position vector
		vec3 a = transformed_face_vertices[0];
		vec3 b = transformed_face_vertices[1];
		vec3 c = transformed_face_vertices[2];

		vec3 ab = vec3_sub(b, a);
		vec3 ac = vec3_sub(c, a);

		// normal is normalised
		vec3 normal = vec3_normalise(vec3_cross_prod(ab, ac));

		vec3 camera_ray = vec3_sub(camera_position, a);
		
		float back_face_cull_dot_prod = vec3_dot(normal, camera_ray);
		if(!(flags & F_BACK_FACE_CULLING))
			back_face_cull_dot_prod = 1.0;
		

		int should_render_face = (back_face_cull_dot_prod > 0.0);
		// for(int i = 0; i < 3; i++) {
		// 	if(transformed_face_vertices[i].z <= (camera_position.z + 5.0)) {
		// 		should_render_face = 0;
		// 	}
		// }
		// if we should render the current face, project face vertices
		// into a triangle to be rendered
		if(should_render_face) {
			Triangle triangle;
			triangle.avg_depth = 0.0;
			for(int j = 0; j < 3; j++) {
				vec4 projected_point = mat4_project_and_normalise(vec4_from_vec3(transformed_face_vertices[j]), projection_matrix);
				// vec2 projected_point = project(transformed_face_vertices[j]);
				projected_point.x *= (window_width / 2.0);
				projected_point.y *= (window_height / 2.0);

				projected_point.x = (window_width / 2.0) + projected_point.x;
				projected_point.y = (window_height / 2.0) + projected_point.y;
				triangle.points[j] = (vec2) { .x = projected_point.x, .y = projected_point.y };
				
				triangle.avg_depth += transformed_face_vertices[j].z * (1.0 / 3.0);
			}
		
			// this is gonna assume global illumination direction is a unit vector
			// normal is normalised above, so also unit vector
			float global_illumination_dot_prod = -vec3_dot(normal, lighting.global_illumination_direction);
			float light_intensity = (global_illumination_dot_prod >= 0.0) ? global_illumination_dot_prod : 0.0;
			uint32_t tri_color = grayscale_of_intensity(light_intensity, 0x55, 0xDD);
			
			triangle.col = tri_color;
			triangles_to_render[triangle_count++] = triangle;
		}
	}
}

void render() {
	draw_gradient(0xFF0066FF, 0xFFFFFFFF);

	if(flags & F_SORT_Z_DEPTH) {
		merge_sort_triangles(triangles_to_render, triangles_to_render_scratch, triangle_count);
	}

	for(int i = 0; i < triangle_count; i++) {
		Triangle triangle = triangles_to_render[i];

		if(flags & F_FILL) {
			fill_triangle(
				triangle.col,
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
