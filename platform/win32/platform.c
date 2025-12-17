#include "../../src/platform.h"
#include "../../src/main.h"
#include "../../src/util.h"

#include <windows.h>
#include <winuser.h>
#include <synchapi.h>

#define WIN32_BUILD

uint32_t controller;
uint32_t flags;

uint32_t *color_buffer = NULL;
int window_width;
int window_height;
int running = 0;

typedef struct WindowBuffer
{
	BITMAPINFO bitmap_info;
	void* data;
	int bytes_per_pixel;
	int pitch;
	int width;
	int height;
}WindowBuffer;

static WNDCLASS wc;
HWND hwnd;
static int my_nCmdShow;
static WindowBuffer window_buffer;
static HDC handle_device_context;
static ticks_per_second;

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
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
				WORD vkCode = LOWORD(wParam);                                 // virtual-key code
				
				WORD keyFlags = HIWORD(lParam);

				WORD scanCode = LOBYTE(keyFlags);                             // scan code
				BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
				
				if (isExtendedKey)
						scanCode = MAKEWORD(scanCode, 0xE0);

				BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
				WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message

				BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;             // transition-state flag, 1 on keyup

				// if we want to distinguish these keys:
				switch (vkCode)
				{
					case 'Q':
					{
						running = 0;
					}break;
					case 'W':
					{
						controller = flag_set(controller, CONTROLLER_W, !isKeyReleased);
					}break;
					case 'A':
					{
						controller = flag_set(controller, CONTROLLER_A, !isKeyReleased);
					}break;
					case 'S':
					{
						controller = flag_set(controller, CONTROLLER_S, !isKeyReleased);
					}break;
					case 'D':
					{
						controller = flag_set(controller, CONTROLLER_D, !isKeyReleased);
					}break;
					case 'H':
					{
						controller = flag_set(controller, CONTROLLER_H, !isKeyReleased);
					}break;
					case 'J':
					{
						controller = flag_set(controller, CONTROLLER_J, !isKeyReleased);
					}break;
					case 'K':
					{
						controller = flag_set(controller, CONTROLLER_K, !isKeyReleased);
					}break;
					case 'L':
					{
						controller = flag_set(controller, CONTROLLER_L, !isKeyReleased);
					}break;
				}

				// ...
		}
		break;
		default:
		{
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

void resizeBuffer(WindowBuffer *wb, int width, int height) {
	if(wb->data) {
		VirtualFree(wb->data, 0, MEM_RELEASE);
	}

	wb->width = width;
	wb->height = height;
 
	wb->bytes_per_pixel = 4;
	wb->bitmap_info.bmiHeader.biSize = sizeof(wb->bitmap_info.bmiHeader);
	wb->bitmap_info.bmiHeader.biWidth	= wb->width;
	wb->bitmap_info.bmiHeader.biHeight = -wb->height; // TODO: is this right?
	wb->bitmap_info.bmiHeader.biPlanes = 1;
	wb->bitmap_info.bmiHeader.biBitCount = 32;
	wb->bitmap_info.bmiHeader.biCompression = BI_RGB;

	int bitmap_mem_size = (wb->width * wb->height) * wb->bytes_per_pixel;
	wb->data = VirtualAlloc(0, bitmap_mem_size, MEM_COMMIT, PAGE_READWRITE);

	wb->pitch = width*wb->bytes_per_pixel;

	color_buffer = (uint32_t *)wb->data;
}

// required platform specific functions
int init_window(int width, int height, int fullscreen) {

	hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		"pikuma3d",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT, width, height,

		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if(!hwnd) {
		printf("Problem getting window handle hwnd");
		return -1;
	}

	running = 1;
	ShowWindow(hwnd, my_nCmdShow);
	handle_device_context = GetDC(hwnd);
	// windowResized(hwnd);
	resizeBuffer(&window_buffer, width, height);

	window_width = width;
	window_height = height;

	return 0;
}


void process_events() {
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void destroy_window() {
	PostQuitMessage(0);
	running = 0;
}

void display_color_buffer() {
	StretchDIBits(
			handle_device_context,
			0, //xDest
			0, //yDest
			window_width,
			window_height,
			0, //xSrc
			0, //ySrc
			window_width,
			window_height,
			color_buffer,
			&window_buffer.bitmap_info, //lpbmi
			DIB_RGB_COLORS, //iUsage
			SRCCOPY //directly copy source to dest. see rop in https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-bitblt
		);
}

// platform specific time functions
void wait_ticks_ms(int ms) {
	Sleep(ms);
}

int	get_ticks_ms() {
	LARGE_INTEGER ticks;
	if(!QueryPerformanceCounter(&ticks)) {
		OutputDebugStringA("QueryPerformanceCounter Failed");
	}
	return ticks.QuadPart;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	memset(&wc, 0, sizeof(WNDCLASS));
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Pikuma3d Window Class";
	RegisterClass(&wc);
	
	my_nCmdShow = nCmdShow;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	ticks_per_second = frequency.QuadPart;

	renderer_main();
}
