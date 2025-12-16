#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "main.h"
#include "platform.h"
#include "renderer.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "lighting.h"
#include "array.h"

#ifndef M_PI
double M_PI = 3.14159265358979323846;
#endif

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

enum {
	FRUSTUM_LEFT,
	FRUSTUM_RIGHT,
	FRUSTUM_TOP,
	FRUSTUM_BOTTOM,
	FRUSTUM_NEAR,
	FRUSTUM_FAR,
	n_frustum_planes
} Frustum_Sides; 

typedef struct {
	vec3 point;
	vec3 normal;
} Plane;

static float MS_PER_FRAME = (1000.0 / FPS);

const float fov_factor = 840.0;
vec3 camera_position = { 0., 0., 1.};
vec3 camera_target = { 0., 0., 0. };
float camera_theta = M_PI / 2.0;

Lighting lighting;
Texture texture;
mat4 projection_matrix;
Plane frustum_planes[n_frustum_planes];


void rotate_camera_y(float theta) {
	camera_target.x = camera_position.x + cos(theta);
	camera_target.z = camera_position.z + -sin(theta);

	printf("camera_target.x = %f, camera_target.z = %f\n", camera_target.x, camera_target.z);
}

int point_is_inside_plane(Plane p, vec3 point) {
	return vec3_dot(vec3_sub(point, p.point), p.normal) > 0.0;
}

void setup_frustum_planes(float fov, float znear, float zfar) {
	frustum_planes[FRUSTUM_RIGHT] = (Plane){
		{ 0., 0., 0.,},
		{ .x = -cos(fov / 2.0), .y = 0., .z = sin(fov / 2.0) }
	};
	frustum_planes[FRUSTUM_LEFT] = (Plane){
		{ 0., 0., 0.,},
		{ .x = cos(fov / 2.0), .y = 0., .z = sin(fov / 2.0) }
	};

	frustum_planes[FRUSTUM_TOP] = (Plane){
		{ 0., 0., 0.,},
		{ .x = 0, .y = -cos(fov / 2.0), .z = sin(fov / 2.0) }
	};
	frustum_planes[FRUSTUM_BOTTOM] = (Plane){
		{ 0., 0., 0.,},
		{ .x = 0, .y = cos(fov / 2.0), .z = sin(fov / 2.0) }
	};

	frustum_planes[FRUSTUM_NEAR] = (Plane){
		{ 0., 0., znear,},
		{ .x = 0, .y = 0.0, .z = 1. }
	};
	frustum_planes[FRUSTUM_FAR] = (Plane){
		{ 0., 0., zfar,},
		{ .x = 0, .y = 0.0, .z = -1. }
	};
}

vec2 project(vec3 v3) {
    vec2 projected;
    projected.x = (v3.z == 0.0) ? 0.0 : (fov_factor * v3.x) / v3.z;
    projected.y = (v3.z == 0.0) ? 0.0 : (fov_factor * v3.y) / v3.z;
    return projected;
}

void setup() {
	
	init_renderer(WIDTH, HEIGHT);
	// Texture png_texture;
	load_texture_from_file("assets/f22.png", &texture);
	// load_cube_mesh_data();
	load_obj_file("assets/f22.obj", &mesh);

	rotate_camera_y(camera_theta);
	// set_redbrick_texture(&texture);
	triangles_to_render = (Triangle *) malloc(mesh.face_count * sizeof(Triangle));
	triangles_to_render_scratch = (Triangle *) malloc(mesh.face_count * sizeof(Triangle));
		
	triangle_count = 0;
	

	// default flags
	{
		flags = 0x00000000;
		flags = (flags | F_ROTATE);
		flags = (flags | F_BACK_FACE_CULLING);
		flags = (flags | F_FILL);
		// flags = (flags | F_DRAW_TEXTURE);
		flags = (flags | F_SORT_Z_DEPTH);
	}

	float fov = M_PI / 3.0; // 60deg
	float aspect_ratio = ((float)window_height / (float)window_width);
	float znear = 5.0;
	float zfar = 100.0;


	projection_matrix = mat4_projection_matrix(
		fov,
		aspect_ratio,
		znear,
		zfar
	);
	
	vec3 lighting_dir;
	lighting_dir.x = 1.0;
	lighting_dir.y = 0.0;
	lighting_dir.z = 1.0;
	lighting.global_illumination_direction = vec3_normalise(lighting_dir);
}

void camera_update() {
	float camera_speed = 0.05;
	float th = (M_PI) - camera_theta;
	float dz = sin(th) * camera_speed;
	float dx = cos(th) * camera_speed;
	if (controller & CONTROLLER_W) {
		printf("forward\n");
		camera_position.z -= dz;
		camera_target.z -= dz;
		camera_position.x -= dx;
		camera_target.x -= dx;
	}
	if (controller & CONTROLLER_S) {
		printf("back\n");
		camera_position.z += dz;
		camera_target.z += dz;
		camera_position.x += dx;
		camera_target.x += dx;
	}
	if (controller & CONTROLLER_A) {
		printf("left\n");
		th = th - (M_PI / 2.0);
		dz = sin(th) * camera_speed;
		dx = cos(th) * camera_speed;
		camera_position.z += dz;
		camera_target.z += dz;
		camera_position.x += dx;
		camera_target.x += dx;
	}
	if (controller & CONTROLLER_D) {
		printf("left\n");
		th = th - (M_PI / 2.0);
		dz = sin(th) * camera_speed;
		dx = cos(th) * camera_speed;
		camera_position.z -= dz;
		camera_target.z -= dz;
		camera_position.x -= dx;
		camera_target.x -= dx;
	}
	if (controller & CONTROLLER_L) {
		printf("r right\n");
		camera_theta += camera_speed;
		rotate_camera_y(camera_theta);
	}
	if (controller & CONTROLLER_H) {
		printf("r left\n");
		camera_theta -= camera_speed;
		if(camera_theta < 0.0) {
			camera_theta += (2 * M_PI);
		}
		rotate_camera_y(camera_theta);
	}
}

void update() {

	for(int i = 0; i < window_width * window_height; i++) {
		z_buffer[i] = 100000.0;
	}

	camera_update();

	if(flags & F_ROTATE) {
		mesh.rotation.x += 0.008;
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

	mat4 view_matrix = mat4_look_at(camera_position, camera_target, (vec3){0., 1., 0});
	transform = mat4_mul(view_matrix, transform);

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

		float bound = 0.1;
		if(a.z <= bound|| b.z <= bound || c.z <= bound) {
			should_render_face = 0;
		}

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
				triangle.points[j] = projected_point;
				
				triangle.avg_depth += transformed_face_vertices[j].z * (1.0 / 3.0);
			}
		
			// this is gonna assume global illumination direction is a unit vector
			// normal is normalised above, so also unit vector
			float global_illumination_dot_prod = -vec3_dot(normal, lighting.global_illumination_direction);
			float light_intensity = (global_illumination_dot_prod >= 0.0) ? global_illumination_dot_prod : 0.0;
			uint32_t tri_color = grayscale_of_intensity(light_intensity, 0x55, 0xDD);
			
			triangle.intensity = light_intensity;
			triangle.col = tri_color;
			triangle.tex_coords[0] = mesh.tex_uv[face.a_uv - 1];
			triangle.tex_coords[1] = mesh.tex_uv[face.b_uv - 1];
			triangle.tex_coords[2] = mesh.tex_uv[face.c_uv - 1];
			triangles_to_render[triangle_count++] = triangle;
		}
	}
}

void render() {

	draw_gradient(0xFF111111, 0xFF888888);

	if(flags & F_SORT_Z_DEPTH) {
		merge_sort_triangles(triangles_to_render, triangles_to_render_scratch, triangle_count);
	}

	for(int i = 0; i < triangle_count; i++) {
		Triangle triangle = triangles_to_render[i];
		
		if(flags & F_DRAW_TEXTURE) {
			textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.tex_coords[0].u, triangle.tex_coords[0].v,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.tex_coords[1].u, triangle.tex_coords[1].v,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.tex_coords[2].u, triangle.tex_coords[2].v,
				texture, triangle.intensity
			);
		} else if(flags & F_FILL) {
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
				0xFFFFFFFF,
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y
			);
		}
	}

	display_color_buffer();
}

int renderer_main() {
	init_window(WIDTH, HEIGHT, 0);
	setup();

	int last_ms = get_ticks_ms();
	while(running) {
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
	return 0;
}
