#include "windows_main.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <winuser.h>
#include <assert.h>

#define WIN_APP_BANNER_HEIGHT 32
RECT window_rect;
static WindowBuffer window_buffer;
static WindowDimension window_dim;
static Rect rect_state;


void win32DisplayBuffer(HDC handle_device_context, WindowBuffer *b) {
	StretchDIBits(
			handle_device_context,
			0, //xDest
			0, //yDest
			window_dim.width, //DestWidth
			window_dim.height, //DestHeight
			0, //xSrc
			0, //ySrc
			b->width, //SrcWidth
			b->height, //SrcHeight
			b->data, //lpBits (data to render)
			&b->bitmap_info, //lpbmi
			DIB_RGB_COLORS, //iUsage
			SRCCOPY //directly copy source to dest. see rop in https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-bitblt
		);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	LRESULT result = 0;

	switch (uMsg)
	{
		case WM_DESTROY:
		case WM_QUIT:
		case WM_CLOSE:
		{
				PostQuitMessage(0);
				running = 0;
				return 0;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			int VKCode = wParam;
			char was_down = ((wParam & (1 << 30)) != 0);
			char is_down = ((wParam & (1 << 31)) == 0);
			if(is_down && !was_down){
				
				if(VKCode == 'W') {
				}
				else if(VKCode == 'A') {
				}
				else if(VKCode == 'S') {
				}
				else if(VKCode == 'D') {
				}
				else if(VKCode == 'Q') {
					PostQuitMessage(0);
					running = 0;
					return 0;
				}
				else if(VKCode == 'E') {
				}
				else if(VKCode == 'R') {
				}
				else if(VKCode == 'C') {
				}
				else if(VKCode == VK_UP) {
				}
				else if(VKCode == VK_LEFT) {
				}
				else if(VKCode == VK_DOWN) {
				}
				else if(VKCode == VK_RIGHT) {
				}
				else if(VKCode == VK_ESCAPE) {
				}
				else if(VKCode == VK_SPACE) {
				}
			}
		}
		default:
		{
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

void win32ProcessMessages(HWND hwnd)
{
	//do the msg loop
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

inline long getTicks()
{
	LARGE_INTEGER ticks;
	if(!QueryPerformanceCounter(&ticks))
	{
		OutputDebugStringA("QueryPerformanceCounter Failed");
	}
	return ticks.QuadPart;
}


void windowResized(HWND hwnd) {
	GetWindowRect(hwnd, &window_rect);
	int w_width = window_rect.right - window_rect.left;
	int w_height = window_rect.bottom - window_rect.top;

	window_dim.top = window_rect.top;
	window_dim.left = window_rect.left;
	window_dim.width = w_width;
	window_dim.height = w_height;
}

void resizeBuffer(WindowBuffer *wb, int width, int height)
{
	if(wb->data)
	{
		VirtualFree(wb->data, 0, MEM_RELEASE);
	}

	wb->width = width;
	wb->height = height;
 
	wb->bytes_per_pixel = 4;
	wb->bitmap_info.bmiHeader.biSize = sizeof(wb->bitmap_info.bmiHeader);
	wb->bitmap_info.bmiHeader.biWidth	= wb->width;
	wb->bitmap_info.bmiHeader.biHeight = wb->height; // TODO: is this right?
	wb->bitmap_info.bmiHeader.biPlanes = 1;
	wb->bitmap_info.bmiHeader.biBitCount = 32;
	wb->bitmap_info.bmiHeader.biCompression = BI_RGB;

	int bitmap_mem_size = (wb->width * wb->height) * wb->bytes_per_pixel;
	wb->data = VirtualAlloc(0, bitmap_mem_size, MEM_COMMIT, PAGE_READWRITE);

	wb->pitch = width*wb->bytes_per_pixel;
}

void render_black() {
	int bytes_per_pixel = 4;
	int width = window_buffer.width;
	int height = window_buffer.height;
	int pitch = window_buffer.pitch;

	unsigned char *row = (unsigned char *)window_buffer.data;
	
	for(int y = 0; y < height; y++)
	{
		unsigned char *pixel = row;
		for(int x = 0; x < width; x++)
		{
			*pixel = 0xFF; pixel++;
			*pixel = 0xFF; pixel++;
			*pixel = 0xFF; pixel++;
			*pixel = 0xFF; pixel++;
		}
		row += pitch;
	}
}

void draw_pixel(uint32_t col, int x, int y) {
	assert(0 <= x && x < window_buffer.width);
	assert(0 <= y && y < window_buffer.height);

	y = window_buffer.height - 1 - y;
	int index = (window_buffer.width * y + x); // assuming 4 bytes per pixel in buffer

	uint32_t *pixel = ((uint32_t *)window_buffer.data + index);
	*pixel = col;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Pikuma3d Window Class";

	RegisterClass(&wc);

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	int ticks_per_second = frequency.QuadPart;

	//Lets create a window
	
	HWND hwnd = CreateWindowEx(
		0, //optional window styles
		wc.lpszClassName, //class name
		"pikuma3d", //window name
		WS_OVERLAPPEDWINDOW, //dwStyle
		
		//default size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,

		NULL, //parent window
		NULL, //menu
		hInstance, //Instance handle
		NULL //additional data
	);

	if(!hwnd)
	{
		printf("Problem getting window handle hwnd");
		return -1;
	}

	running = 1;

	ShowWindow(hwnd, nCmdShow);
	HDC hdc = GetDC(hwnd);
	windowResized(hwnd);
	resizeBuffer(&window_buffer, window_dim.width, window_dim.height);
	
	int prev_ticks = getTicks();

	float s_per_frame = 1.0 / 60.0;
	float accumulator = 0.0;
	
	float start_x = 0.0;
	while(running)
	{

		win32ProcessMessages(hwnd);

		POINT point;
		GetCursorPos(&point);
		
		RECT rect;
		GetWindowRect(hwnd, &rect);

		int mouseWindowPosY = point.y - rect.top - WIN_APP_BANNER_HEIGHT;
		int mouseWindowPosX = point.x - rect.left;

		char output_msg[100];
		
		LPCTSTR cursor = IDC_HAND;
		HCURSOR hCursor = LoadCursor(NULL, cursor);
		SetCursor(hCursor);

		if(accumulator > s_per_frame)
		{
			// render_black();
			for(int y = 20; y < 40; y++) {
				for(int x = round(start_x); x < round(start_x) + 20; x++) {
					draw_pixel(0xFF00FF00, x, y);
				}
			}
			win32DisplayBuffer(hdc, &window_buffer);
			accumulator = 0.0;
		}

		int current_ticks = getTicks();
		int d_ticks = current_ticks - prev_ticks;
		float dt = ((float)d_ticks / (float)ticks_per_second);

		accumulator += dt;
		start_x += 0.001;

		prev_ticks = current_ticks;
	}

	return 0;
}
