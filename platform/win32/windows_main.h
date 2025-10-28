#ifndef WINDOWS_MAIN
#define WINDOWS_MAIN
#include <windows.h>

typedef struct Rect 
{
	int x;
	int y;
	int width;
	int height;
}Rect;

typedef struct WindowBuffer
{
	BITMAPINFO bitmap_info;
	void* data;
	int bytes_per_pixel;
	int pitch;
	int width;
	int height;
}WindowBuffer;

typedef struct WindowDimension
{
	int top;
	int left;
	int width;
	int height;
}WindowDimension;

extern char running;
extern RECT window_rect;
extern WindowBuffer window_buffer;
extern WindowDimension window_dim;
extern Rect rect_state;

draw_pixel(uint32_t col, int x, int y);
#endif

