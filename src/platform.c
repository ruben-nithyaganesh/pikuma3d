#include "platform.h"

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
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_BORDERLESS
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
				if(event.key.keysym.sym == SDLK_q) {
					running = 0;
				}
			}break;
			default:
				break;
		}
	}
}

void draw_pixel(uint32_t value, int x, int y) {
	if(x >= 0 && x < window_width && y >= 0 && y < window_height) {
		color_buffer[(window_width * y) + x] = value;
	}
}

void draw_rect(uint32_t value, int top, int left, int width, int height) {
	for(int y = top; y < top + height; y ++) {
		for(int x = left; x < left + width; x ++) {
            draw_pixel(value, x, y);
		}
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

int get_ticks_ms() {
	return SDL_GetTicks();
}

void wait_ticks_ms(int ms) {
	SDL_Delay(ms);
	// int current_ms = get_ticks_ms();
	// while(!SDL_TICKS_PASSED(get_ticks_ms() - current_ms, ms));
}

void render_clear() {
	SDL_RenderClear(renderer);
}

void render_present() {
	SDL_RenderPresent(renderer);
}

void render_color_buffer() {
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
}

void destroy_window() {
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}