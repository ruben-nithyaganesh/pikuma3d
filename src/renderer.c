#include "renderer.h"
#include "util.h"
#include <math.h>

float *z_buffer;
void init_renderer(int width, int height) {
	z_buffer = (float *)malloc(width * height * sizeof(float));
	for(int i = 0; i < width * height; i++) {
		z_buffer[i] = 100000.0;
	}
}

void draw_pixel(uint32_t value, int x, int y) {
	if(x >= 0 && x < window_width && y >= 0 && y < window_height) {
		color_buffer[(window_width * y) + x] = value;
	}
}

void draw_line(uint32_t value, int x0, int y0, int x1, int y1) {
	x0 = bound(x0, 0, window_width);
	x1 = bound(x1, 0, window_width);
	y0 = bound(y0, 0, window_height);
	y1 = bound(y1, 0, window_height);

	#define DDA
	#ifdef DDA
		int delta_x = x1 - x0;
		int delta_y = y1 - y0;

		int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

		float x_inc = delta_x / (float) side_length;
		float y_inc = delta_y / (float) side_length;

		float current_x = x0;
		float current_y = y0;

		for(int i = 0 ; i <= side_length; i++) {
			draw_pixel(value, round(current_x), round(current_y));
			current_x += x_inc;
			current_y += y_inc;
		}
	#endif
	#ifdef BRESENHAM
		int not_implemented = 0;
	#endif
}

void draw_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2) {
	draw_line(value, x0, y0, x1, y1);
	draw_line(value, x1, y1, x2, y2);
	draw_line(value, x2, y2, x0, y0);
}

vec3 barycentric_weights(vec2 a, vec2 b, vec2 c, vec2 p) {
	vec2 ac = vec2_sub(c, a);
	vec2 ab = vec2_sub(b, a);
	vec2 ap = vec2_sub(p, a);
	vec2 pb = vec2_sub(b, p);
	vec2 pc = vec2_sub(c, p);

	float parallelogram_area = (ac.x * ab.y - ac.y * ab.x);

	float alpha = (pc.x * pb.y - pc.y * pb.x) / parallelogram_area;

	float beta = (ac.x * ap.y - ac.y * ap.x) / parallelogram_area;

	float gamma = 1.0 - alpha - beta;

	vec3 res;
	res.x = alpha;
	res.y = beta;
	res.z = gamma;

	return res;
}

uint32_t lerp_pixel(uint32_t c, float i) {
	uint32_t lerped;
	uint8_t r = (c & 0x00FF0000) >> 16;
	uint8_t g = (c & 0x0000FF00) >> 8;
	uint8_t b = (c & 0x000000FF);

	uint8_t lerped_r = (uint8_t)(r * i);
	uint8_t lerped_g = (uint8_t)(g * i);
	uint8_t lerped_b = (uint8_t)(b * i);

	lerped = 0xFF000000 | (lerped_r << 16) | (lerped_g << 8) | (lerped_b);
	return lerped;
}

void draw_texel(
	int x, int y,
	vec4 a, vec4 b, vec4 c,
	float u0, float v0, float u1, float v1, float u2, float v2,
	Texture texture, float intensity
) {
	vec2 p = { x, y };
	vec3 weights = barycentric_weights(
		(vec2){ a.x, a.y },
		(vec2){ b.x, b.y },
		(vec2){ c.x, c.y },
		p
	);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;
	
	interpolated_u = (u0 / a.w) * alpha + (u1 / b.w) * beta + (u2 / c.w) * gamma;
	interpolated_v = (v0 / a.w) * alpha + (v1 / b.w) * beta + (v2 / c.w) * gamma;

	interpolated_reciprocal_w = (1.0 / a.w) * alpha + (1.0 / b.w) * beta + (1.0 / c.w) * gamma;

	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	int tex_x = (int)(texture.width * interpolated_u);
	int tex_y = (int)(texture.height * interpolated_v);
	
	if (z_buffer[window_width * y + x] >= interpolated_reciprocal_w) {
		uint32_t col = texture.data[tex_y * texture.width + tex_x];
		// col = lerp_pixel(col, intensity);
		draw_pixel(col, x, y);
		z_buffer[window_width * y + x] = interpolated_reciprocal_w;
	}
}

void textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	Texture texture, float intensity
) {
	// sort triangle points in order of y
	// y0 < y1 < y2
	if(y1 > y2) { 
		swap_int(&x1, &x2);
		swap_int(&y1, &y2);
		swap_float(&z1, &z2);
		swap_float(&w1, &w2);
		swap_float(&u1, &u2);
		swap_float(&v1, &v2);
	}

	if(y0 > y1) {
		swap_int(&x0, &x1);
		swap_int(&y0, &y1);
		swap_float(&z0, &z1);
		swap_float(&w0, &w1);
		swap_float(&u0, &u1);
		swap_float(&v0, &v1);
	}
	
	if(y1 > y2) {
		swap_int(&x1, &x2);
		swap_int(&y1, &y2);
		swap_float(&z1, &z2);
		swap_float(&w1, &w2);
		swap_float(&u1, &u2);
		swap_float(&v1, &v2);
	}

	vec4 point_a = { x0, y0, z0, w0 };
	vec4 point_b = { x1, y1, z1, w1 };
	vec4 point_c = { x2, y2, z2, w2 };
	
	// get (Mx, y1), intersection of triangle midpoint line
	int Mx = x0;
	if (y2 != y0) {
		Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;
	}

	// flat bottom triangle
	int delta_y = (y1 - y0);
	int delta_x1 = (x1 - x0);
	int delta_x2 = (Mx - x0);
	
	float inv_slope_1 = (delta_y == 0) ? 0.0 : ((float) delta_x1 / (float) delta_y);
	float inv_slope_2 = (delta_y == 0) ? 0.0 : ((float) delta_x2 / (float) delta_y);
	
	float x_start = (float)x0 + inv_slope_1;
	float x_end = (float)x0 + inv_slope_2;

	if(x_start > x_end) {
		swap_float(&x_start, &x_end);
		swap_float(&inv_slope_1, &inv_slope_2);
	}

	for(int y = y0 + 1; y <= y1; y++) {

		for(int x = round(x_start); x <= round(x_end); x++) {
			
			draw_texel(
				x, y,
				point_a, point_b, point_c,
				u0, v0, u1, v1, u2, v2,
				texture, intensity
			);

			// if(x % 2 == 0) {
			// 	draw_pixel(0xFFFF0000, x, y);
			// } else {
			// 	draw_pixel(0xFF000000, x, y);
			// }
		}

		x_start += inv_slope_1;
		x_end += inv_slope_2;

	}

	// flat top triangle
	delta_y = (y2 - y1);
	delta_x1 = (x2 - x1);
	delta_x2 = (x2 - Mx);

	inv_slope_1 = (delta_y == 0) ? 0.0 : ((float) delta_x1 / (float) delta_y);
	inv_slope_2 = (delta_y == 0) ? 0.0 : ((float) delta_x2 / (float) delta_y);

	x_start = (float)x2 - inv_slope_1;
	x_end = (float)x2 - inv_slope_2;

	if(x_start > x_end) {
		swap_float(&x_start, &x_end);
		swap_float(&inv_slope_1, &inv_slope_2);
	}
	
	for(int y = y2 - 1; y >= y1; --y) {
		for(int x = round(x_start); x <= round(x_end); x++) {
			draw_texel(
				x, y,
				point_a, point_b, point_c,
				u0, v0, u1, v1, u2, v2,
				texture, intensity
			);
		}

		x_start -= inv_slope_1;
		x_end -= inv_slope_2;

	}
}

void draw_flat_bottom_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2) {
	int delta_x1 = (x1 - x0);
	int delta_y1 = (y1 - y0);
	int delta_x2 = (x2 - x0);
	int delta_y2 = (y2 - y0);

	float x_1_inc = (delta_y1 == 0) ? 0.0 : ((float) delta_x1 / (float) delta_y1);
	float x_2_inc = (delta_y2 == 0) ? 0.0 : ((float) delta_x2 / (float) delta_y2);
	
	float x_start = x0;
	float x_end = x0;

	for(int i = y0 + 1; i <= y1; i++) {
		x_start += x_1_inc;
		x_end += x_2_inc;

		int x_start_int = round(x_start);
		int x_end_int = round(x_end);

		int start = m_min(x_start_int, x_end_int);
		int len = abs(x_start_int - x_end_int);
		for(int j = 0; j <= len; j++)
			draw_pixel(value, start + j, i);

	}
}

void draw_flat_top_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2) {
	int delta_x1 = (x2 - x0);
	int delta_y1 = (y2 - y0);
	int delta_x2 = (x2 - x1);
	int delta_y2 = (y2 - y1);

	float x_1_inc = (delta_y1 == 0) ? 0.0 : ((float) delta_x1 / (float) delta_y1);
	float x_2_inc = (delta_y2 == 0) ? 0.0 : ((float) delta_x2 / (float) delta_y2);

	float x_start = x2;
	float x_end = x2;
	
	for(int i = y2 - 1; i >= y1; --i) {
		x_start -= x_1_inc;
		x_end -= x_2_inc;

		int x_start_int = round(x_start);
		int x_end_int = round(x_end);

		int start = m_min(x_start_int, x_end_int);
		int len = abs(x_start_int - x_end_int);
		for(int j = 0; j <= len; j++)
			draw_pixel(value, start + j, i);
	}
}

void fill_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2) {
	// sort triangle points in order of y
	// y0 < y1 < y2
	if(y1 > y2) { 
		swap_int(&x1, &x2);
		swap_int(&y1, &y2);
	}

	if(y0 > y1) {
		swap_int(&x0, &x1);
		swap_int(&y0, &y1);
	}
	
	if(y1 > y2) {
		swap_int(&x1, &x2);
		swap_int(&y1, &y2);
	}
	
	// get (Mx, My), intersection of triangle midpoint line
	int My = y1;
	int Mx = x0;
	if (y2 != y0) {
		Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;
	}

	draw_flat_bottom_triangle(value, x0, y0, x1, y1, Mx, My);
	draw_flat_top_triangle(value, x1, y1, Mx, My, x2, y2);
}

void draw_rect(uint32_t value, int top, int left, int width, int height) {
	for(int y = top; y < top + height; y ++) {
		for(int x = left; x < left + width; x ++) {
            draw_pixel(value, x, y);
		}
	}
}

void draw_gradient(uint32_t start, uint32_t end) {
	
	uint8_t a_start = (start & 0xFF000000) >> 24;
	uint8_t r_start = (start & 0x00FF0000) >> 16;
	uint8_t g_start = (start & 0x0000FF00) >> 8;
	uint8_t b_start = (start & 0x000000FF);

	uint8_t a_end = (end & 0xFF000000) >> 24;
	uint8_t r_end = (end & 0x00FF0000) >> 16;
	uint8_t g_end = (end & 0x0000FF00) >> 8;
	uint8_t b_end = (end & 0x000000FF);

	float a_inc = (a_end - a_start) / (float)window_height;
	float r_inc = (r_end - r_start) / (float)window_height;
	float g_inc = (g_end - g_start) / (float)window_height;
	float b_inc = (b_end - b_start) / (float)window_height;
	
	float a, r, g, b;
	a = (float)a_start;
	r = (float)r_start;
	g = (float)g_start;
	b = (float)b_start;

	uint32_t col = start;
	for(int y = 0; y < window_height; y++) {
		for(int x = 0; x < window_width; x++) {
			draw_pixel(col, x, y);
		}
		
		a += a_inc;
		r += r_inc;
		g += g_inc;
		b += b_inc;
		col = ((uint32_t)a << 24) + ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
	}
}

void draw_grid() {
	for(int y = 0; y < window_height; y += 10) {
		for(int x = 0; x < window_width; x += 20) {
			draw_rect(0xFF000000 + ((y % 20 == 0) * 0x00444444), y, x, 10, 10);
			draw_rect(0xFF000000 + ((y % 20 != 0) * 0x00444444), y, x + 10, 10, 10);
		}
	}
}

