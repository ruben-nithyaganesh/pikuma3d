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
#include "util.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#ifndef WIN32_BUILD
	#define WIDTH 1280
	#define HEIGHT 720
#endif
#ifdef WIN32_BUILD
	#define WIDTH 1920
	#define HEIGHT 1080
#endif

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
vec3 camera_position = { 0., 0., 2.};
vec3 camera_target = { 0., 0., 0. };
float camera_theta = M_PI / 2.0;

Lighting lighting;
Texture texture;
mat4 projection_matrix;
Plane frustum_planes[n_frustum_planes];

int mesh_count;
Mesh *meshes;

void rotate_camera_y(float theta) {
	camera_target.x = camera_position.x + cos(theta);
	camera_target.z = camera_position.z + -sin(theta);

	printf("camera_target.x = %f, camera_target.z = %f\n", camera_target.x, camera_target.z);
}

int point_is_inside_plane(vec3 point, Plane p) {
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

typedef struct { vec4 *points; int size; } Polygon;

Polygon polygon_from_triangle(Triangle t) {
	Polygon p;
	p.size = 3;
	p.points = (vec4 *) malloc(sizeof(vec4) * 3);
	p.points[0] = t.points[0];
	p.points[1] = t.points[1];
	p.points[2] = t.points[2];
	return p;
}

float get_intersection_t(vec4 inside, vec4 outside, Plane p) {
	vec3 q1 = vec3_from_vec4(inside);
	vec3 q2 = vec3_from_vec4(outside);

	float dot_q1 = vec3_dot(p.normal, vec3_sub(q1, p.point));
	float dot_q2 = vec3_dot(p.normal, vec3_sub(q2, p.point));
	
	if (dot_q1 == dot_q2) return 0.0;

	float t = dot_q1 / (dot_q1 - dot_q2);
	
	return t;
}

vec4 get_intersection_with_plane(vec4 inside, vec4 outside, Plane p) {
	float t = get_intersection_t(inside, outside, p);
	if (t <= 0.0) return inside;
	if (t >= 1.0) return outside;
	
	float x, y;
	vec4 result;
	x = inside.x;
	y = outside.x;
	result.x = mylerp(x, y, t);
	result.y = mylerp(inside.y, outside.y, t);
	result.z = mylerp(inside.z, outside.z, t);
	result.w = mylerp(inside.w, outside.w, t);
	return result;
	
}

Polygon clip_polygon_against_plane(Polygon polygon, Plane plane) {
	vec4 *inside = (vec4 *) malloc(sizeof(vec4) * (polygon.size + 1));
	int clipped_size = 0;

	vec4 prev = polygon.points[0];
	int prev_was_inside = point_is_inside_plane(vec3_from_vec4(prev), plane);
	if (prev_was_inside) inside[clipped_size++] = prev;

	for (int i = 1; i < polygon.size + 1; i++) {
		int index = i < polygon.size ? i : 0;
		vec4 next = polygon.points[(i < polygon.size) ? i : 0];
		int next_is_inside = point_is_inside_plane(vec3_from_vec4(next), plane);

		if (prev_was_inside && next_is_inside) {
			if (index != 0) inside[clipped_size++] = next; 
		} else if (prev_was_inside && !next_is_inside) {
			vec4 intersect = get_intersection_with_plane(prev, next, plane);
			inside[clipped_size++] = intersect;
		} else if (!prev_was_inside && next_is_inside) {
			vec4 intersect = get_intersection_with_plane(prev, next, plane);
			inside[clipped_size++] = intersect;

			// if index = 0 and 'next' is inside, this is the first
			// point in the polygon that we would've already added
			if (index != 0) inside[clipped_size++] = next; 
		} else { } // !prev_was_inside && !next_was_inside
		
		prev = next;
		prev_was_inside = next_is_inside;
	}
	
	assert(clipped_size <= (polygon.size + 1));
	Polygon clipped;
	clipped.points = inside;
	clipped.size = clipped_size;
	return clipped;
}

Polygon clip_frustums(Triangle t) {
	Polygon p;
	p.size = 3;
	p.points = (vec4 *)malloc(sizeof(vec4) * p.size);
	p.points[0] = t.points[0];
	p.points[1] = t.points[1];
	p.points[2] = t.points[2];

	p = clip_polygon_against_plane(p, frustum_planes[0]);
	p = clip_polygon_against_plane(p, frustum_planes[1]);
	p = clip_polygon_against_plane(p, frustum_planes[2]);
	p = clip_polygon_against_plane(p, frustum_planes[3]);
	p = clip_polygon_against_plane(p, frustum_planes[4]);
	p = clip_polygon_against_plane(p, frustum_planes[5]);

	// assert(p.size <= 3);
}

void print_vec4(vec4 v) {
	printf("%.2f, %.2f, %.2f, %.2f\n", v.x, v.y, v.z, v.w);
}

void setup() {
		
	mesh_count = 3;
	meshes = (Mesh *)malloc(sizeof(Mesh) * mesh_count);

	init_renderer(WIDTH, HEIGHT);
	// Texture png_texture;
	load_texture_from_file("assets/f22.png", &texture);
	// load_cube_mesh_data();
	

	load_obj_file("assets/f22.obj", &(meshes[0]));
	mesh_count = 1;
	// load_obj_file("assets/f22.obj", &(meshes[1]));
	// load_obj_file("assets/f22.obj", &(meshes[2]));

	rotate_camera_y(camera_theta);
	// set_redbrick_texture(&texture);

	int face_count = 0;
	for(int i = 0; i < mesh_count; i++) {
		face_count += meshes[i].face_count;
		meshes[i].scale = (vec3) { .1, .1, .1 };
		meshes[i].translation = (vec3) { .0, .0, -1 };
		meshes[i].rotation = (vec3) { .0, .0, .0 };
	}

	meshes[1].translation.x = 2.0;
	meshes[2].translation.x = 4.0;
	
	triangles_to_render = (Triangle *) malloc(10 * face_count * sizeof(Triangle));
	triangles_to_render_scratch = (Triangle *) malloc(10 * face_count * sizeof(Triangle));
		
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
	float znear = 0.1;
	float zfar = 10.0;


	projection_matrix = mat4_projection_matrix(
		fov,
		aspect_ratio,
		znear,
		zfar
	);

	setup_frustum_planes(fov, znear, zfar);
	
	vec3 lighting_dir;
	lighting_dir.x = 1.0;
	lighting_dir.y = 0.0;
	lighting_dir.z = 1.0;
	lighting.global_illumination_direction = vec3_normalise(lighting_dir);


	Polygon p;
	p.size = 3;
	p.points = (vec4 *)malloc(sizeof(vec4) * p.size);
	p.points[0] = (vec4) { .x = 1., .y = 1., .z = 0.0, .w = 0.0 };
	p.points[1] = (vec4) { .x = -1., .y = 1., .z = 0.0, .w = 0.0 };
	p.points[2] = (vec4) { .x = 2., .y = -1., .z = 0.0, .w = 0.0 };

	Plane plane;
	plane.point = (vec3) { .x = 0.0, .y = 0.0, .z = 0.0 };
	plane.normal = (vec3) { .x = 0.0, .y = 1.0, .z = 0.0 };
	
	Polygon clipped = clip_polygon_against_plane(p, plane);
	for (int i = 0; i < clipped.size; i++) {
		vec4 v = clipped.points[i];
		print_vec4(clipped.points[i]);
	}
}

void camera_update() {
	float camera_speed = 0.025;
	float th = (M_PI) - camera_theta;
	float dz = sin(th) * camera_speed;
	float dx = cos(th) * camera_speed;
	if (controller & CONTROLLER_W) {
		camera_position.z -= dz;
		camera_target.z -= dz;
		camera_position.x -= dx;
		camera_target.x -= dx;
	}
	if (controller & CONTROLLER_S) {
		camera_position.z += dz;
		camera_target.z += dz;
		camera_position.x += dx;
		camera_target.x += dx;
	}
	if (controller & CONTROLLER_A) {
		th = th - (M_PI / 2.0);
		dz = sin(th) * camera_speed;
		dx = cos(th) * camera_speed;
		camera_position.z += dz;
		camera_target.z += dz;
		camera_position.x += dx;
		camera_target.x += dx;
	}
	if (controller & CONTROLLER_D) {
		th = th - (M_PI / 2.0);
		dz = sin(th) * camera_speed;
		dx = cos(th) * camera_speed;
		camera_position.z -= dz;
		camera_target.z -= dz;
		camera_position.x -= dx;
		camera_target.x -= dx;
	}
	if (controller & CONTROLLER_L) {
		camera_theta += camera_speed;
		rotate_camera_y(camera_theta);
	}
	if (controller & CONTROLLER_H) {
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
	mat4 view_matrix = mat4_look_at(camera_position, camera_target, (vec3){0., 1., 0});

	triangle_count = 0;

	for (int mesh_index = 0; mesh_index < mesh_count; mesh_index++) {
		Mesh mesh = meshes[mesh_index];

		if(flags & F_ROTATE) {
			meshes[mesh_index].rotation.x += 0.008;
			meshes[mesh_index].rotation.z += 0.008;
		}


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

		// transform = mat4_mul(view_matrix, transform);

		for(int i = 0; i < mesh.face_count; i++) {
			Face face = mesh.faces[i];
			vec3 face_vertices[3];
			face_vertices[0] = mesh.vertices[face.a - 1];
			face_vertices[1] = mesh.vertices[face.b - 1];
			face_vertices[2] = mesh.vertices[face.c - 1];
			
			// transform each vertex of the current face according to the current mesh rotation
			vec3 transformed_face_vertices[3];
			vec3 face_world_space_vertices[3];
			for(int j = 0; j < 3; j++) {
				vec4 transformed_point = vec4_from_vec3(face_vertices[j]);
				transformed_point = mat4_mul_vec4(transform, transformed_point);

				face_world_space_vertices[j] = vec3_from_vec4(transformed_point);

				transformed_point = mat4_mul_vec4(view_matrix, transformed_point);
				transformed_face_vertices[j] = vec3_from_vec4(transformed_point);
			}
			
			// if back face culling is enabled, compute actual dot prod
			// of face normal to camera position vector
			vec3 a = face_world_space_vertices[0];
			vec3 b = face_world_space_vertices[1];
			vec3 c = face_world_space_vertices[2];

			vec3 ab = vec3_sub(b, a);
			vec3 ac = vec3_sub(c, a);

			// normal is normalised
			vec3 normal = vec3_normalise(vec3_cross_prod(ab, ac));

			vec3 camera_ray = vec3_sub(camera_position, a);
			
			float back_face_cull_dot_prod = vec3_dot(normal, camera_ray);
			if(!(flags & F_BACK_FACE_CULLING))
				back_face_cull_dot_prod = 1.0;
			

			int should_render_face = (back_face_cull_dot_prod > 0.0);

			// if we should render the current face, project face vertices
			// into a triangle to be rendered
			if(should_render_face) {
				
				Triangle clipping_triangle;
				clipping_triangle.points[0] = vec4_from_vec3(transformed_face_vertices[0]);
				clipping_triangle.points[1] = vec4_from_vec3(transformed_face_vertices[1]);
				clipping_triangle.points[2] = vec4_from_vec3(transformed_face_vertices[2]);
				Polygon clipped = clip_frustums(clipping_triangle);

				for (int tri_index = 1; tri_index < clipped.size - 1; tri_index++) {
					Triangle triangle;
					triangle.avg_depth = 0.0;

					vec4 tri_points[3];
					tri_points[0] = clipped.points[0];
					tri_points[1] = clipped.points[tri_index];
					tri_points[2] = clipped.points[tri_index + 1];

					for(int j = 0; j < 3; j++) {
						vec4 projected_point = mat4_project_and_normalise(tri_points[j], projection_matrix);
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
