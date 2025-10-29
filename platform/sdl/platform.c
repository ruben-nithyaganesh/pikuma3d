#include <SDL2/SDL.h>
#include "../../src/platform.h"
#include "../../src/main.h"
#include "../../src/util.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *color_buffer_texture;

uint32_t flags = 0;
uint32_t controller = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_width;
int window_height;

int running;

int init_window(int man_width, int man_height, int fullscreen) {

	int width = man_width;
	int height = man_height;

	if(fullscreen != 0) {
		SDL_DisplayMode display_mode;
		if(SDL_GetCurrentDisplayMode(0, &display_mode) == 0) {
			width = display_mode.w;
			height = display_mode.h;
		}
	}

	window_width = width;
	window_height = height;

	printf("w: %d, h: %d\n", width, height);

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initialising SDL.\n");
		return 0;
	}
	
	window = SDL_CreateWindow(
		"pikuma3D",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		0
	);

	if(!window) {
		fprintf(stderr, "Error initialising SDL Window.\n");
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(!renderer) {
		fprintf(stderr, "Error initialising SDL Renderer.\n");
		return 0;
	}

	color_buffer = (uint32_t *) malloc(sizeof(uint32_t) * width * height);

	if(!color_buffer) {
		fprintf(stderr, "Error initialising color buffer.\n");
		return 0;
	}

	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width,
		height
	);

	if(!color_buffer_texture) {
		fprintf(stderr, "Error initialising SDL Texture.\n");
		return 0;
	}

	running = 1;
	return 1;
}

void process_events() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch (event.type)
		{
			case SDL_QUIT:
				running = 0;
			break;

			case SDL_KEYDOWN:
			{
				if(event.key.keysym.sym == SDLK_a) {
					controller = flag_set(controller, C_LEFT, 1);
				}
				else if(event.key.keysym.sym == SDLK_d) {
					controller = flag_set(controller, C_RIGHT, 1);
				}
				else if(event.key.keysym.sym == SDLK_w) {
					controller = flag_set(controller, C_UP, 1);
				}
				else if(event.key.keysym.sym == SDLK_s) {
					controller = flag_set(controller, C_DOWN, 1);
				}
				else if(event.key.keysym.sym == SDLK_j) {
					controller = flag_set(controller, C_LOOK_LEFT, 1);
				}
				else if(event.key.keysym.sym == SDLK_l) {
					controller = flag_set(controller, C_LOOK_RIGHT, 1);
				}
			}break;
			case SDL_KEYUP:
			{
				if(event.key.keysym.sym == SDLK_q) {
					running = 0;
				}
				else if(event.key.keysym.sym == SDLK_r) {
					flags = flag_toggle(flags, F_ROTATE);
				}
				else if(event.key.keysym.sym == SDLK_v) {
					flags = flag_toggle(flags, F_DRAW_VERTICES);
				}
				else if(event.key.keysym.sym == SDLK_b) {
					flags = flag_toggle(flags, F_BACK_FACE_CULLING);
				}
				else if(event.key.keysym.sym == SDLK_e) {
					flags = flag_toggle(flags, F_DRAW_LINES);
				}
				else if(event.key.keysym.sym == SDLK_f) {
					flags = flag_toggle(flags, F_FILL);
				}
				else if(event.key.keysym.sym == SDLK_z) {
					flags = flag_toggle(flags, F_SORT_Z_DEPTH);
				}
				else if(event.key.keysym.sym == SDLK_t) {
					flags = flag_toggle(flags, F_DRAW_TEXTURE);
				}

				// controller stuff
				else if(event.key.keysym.sym == SDLK_a) {
					controller = flag_set(controller, C_LEFT, 0);
				}
				else if(event.key.keysym.sym == SDLK_d) {
					controller = flag_set(controller, C_RIGHT, 0);
				}
				else if(event.key.keysym.sym == SDLK_w) {
					controller = flag_set(controller, C_UP, 0);
				}
				else if(event.key.keysym.sym == SDLK_s) {
					controller = flag_set(controller, C_DOWN, 0);
				}
				else if(event.key.keysym.sym == SDLK_j) {
					controller = flag_set(controller, C_LOOK_LEFT, 0);
				}
				else if(event.key.keysym.sym == SDLK_l) {
					controller = flag_set(controller, C_LOOK_RIGHT, 0);
				}
			}break;
			default:
				break;
		}
	}
}

int get_ticks_ms() {
	return SDL_GetTicks();
}

void wait_ticks_ms(int ms) {
	SDL_Delay(ms);
}

void display_color_buffer() {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);

	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);

	SDL_RenderPresent(renderer);
}

void destroy_window() {
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
	renderer_main();
}

