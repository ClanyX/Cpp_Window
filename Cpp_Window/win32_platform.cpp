#include <windows.h>

//Mutable properties
int window_widht = 960;
int window_height = 540;
// -> Other things can be change in code!

bool running = true;

void* buffer_memory;
int buffer_widht;
int buffer_height;
BITMAPINFO buffer_bitmap_info;

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			buffer_widht = rect.right - rect.left;
			buffer_height = rect.bottom - rect.top;

			int buffer_size = buffer_widht * buffer_height * sizeof(unsigned int);

			if (buffer_memory) VirtualFree(buffer_memory, 0, MEM_RELEASE);
			buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
			buffer_bitmap_info.bmiHeader.biWidth = buffer_widht;
			buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
			buffer_bitmap_info.bmiHeader.biPlanes = 1;
			buffer_bitmap_info.bmiHeader.biBitCount = 32;
			buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	//Windows Class
	WNDCLASS windows_class = {};
	windows_class.style = CS_HREDRAW | CS_VREDRAW;
	windows_class.lpszClassName = L"Game Window Class";
	windows_class.lpfnWndProc = window_callback;

	//Register Class
	RegisterClass(&windows_class);

	//Create Window
	HWND window = CreateWindow(windows_class.lpszClassName, L"My First WINDOW!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, window_widht, window_height, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	while (running) {
		//Input
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//Simulate
		unsigned int* pixel = (unsigned int*)buffer_memory;
		for (int y = 0; y < buffer_height; y++) {
			for (int x = 0; x < buffer_widht; x++)
			{
				*pixel++ = 0xff5500;
			}
		}

		//Render
		StretchDIBits(hdc, 0 ,0, buffer_widht, buffer_height, 0, 0, buffer_widht, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}
}