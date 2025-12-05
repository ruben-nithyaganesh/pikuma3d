#ifndef RENDERER
#define RENDERER

#include "platform.h"
#include "vector.h"
#include "texture.h"

extern float *z_buffer;

void init_renderer();
void draw_grid();
void draw_pixel(uint32_t value, int x, int y);
void draw_gradient(uint32_t start, uint32_t end);
void draw_rect(uint32_t value, int top, int left, int width, int height);
void draw_line(uint32_t value, int x0, int y0, int x1, int y1);
void draw_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2);
void fill_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2);
void textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	Texture texture, float intensity);
void draw_texel(int x, int y, vec4 a, vec4 b, vec4 c, float u0, float v0, float u1, float v1, float u2, float v2, Texture texture, float intensity);

#endif
