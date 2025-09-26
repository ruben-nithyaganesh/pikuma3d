#ifndef PLATFORM
#define PLATFORM

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// Variables
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;

extern int window_width;
extern int window_height;

extern int running;

// Functions

// window
int init_window(int man_width, int man_height, int fullscreen);
void process_events();
void destroy_window();

// rendering
void draw_grid();
void draw_rect(uint32_t value, int top, int left, int width, int height);
void draw_line(uint32_t value, int x0, int y0, int x1, int y1);
void draw_triangle(uint32_t value, int x0, int y0, int x1, int y1, int x2, int y2);
void draw_pixel(uint32_t value, int x, int y);
void render_color_buffer();
void render_present();
void render_clear();

// time
void wait_ticks_ms(int ms);
int	get_ticks_ms();

#endif
