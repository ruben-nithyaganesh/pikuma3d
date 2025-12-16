#ifndef PLATFORM
#define PLATFORM

#include <stdio.h>
#include <stdlib.h>
// #include <SDL2/SDL.h>
#include "vector.h"
#include "texture.h"

#define m_min(a, b) ((a <= b) ? a : b)

extern uint32_t controller;
#define CONTROLLER_W	(0x00000001)
#define CONTROLLER_A	(0x00000001 << 1)
#define CONTROLLER_S	(0x00000001 << 2)
#define CONTROLLER_D	(0x00000001 << 3)
#define CONTROLLER_H	(0x00000001 << 4)
#define CONTROLLER_J	(0x00000001 << 5)
#define CONTROLLER_K	(0x00000001 << 6)
#define CONTROLLER_L	(0x00000001 << 6)

extern uint32_t flags;
#define F_DRAW_VERTICES 	(0x00000001)
#define F_ROTATE 			(0x00000001 << 1)
#define F_BACK_FACE_CULLING (0x00000001 << 2)
#define F_DRAW_LINES		(0x00000001 << 3)
#define F_FILL 				(0x00000001 << 4)
#define F_SORT_Z_DEPTH 		(0x00000001 << 5)
#define F_DRAW_TEXTURE 		(0x00000001 << 6)

extern uint32_t *color_buffer;
extern int window_width;
extern int window_height;
extern int running;

// required platform specific functions
int init_window(int width, int height, int fullscreen);
void process_events();
void destroy_window();
void display_color_buffer();

// platform specific time functions
void wait_ticks_ms(int ms);
int	get_ticks_ms();

#endif
