#ifdef _WIN32

#include <windows.h>
bool hideConsole() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
	return true;
}
bool showConsole() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOW);
	return true;
}

#endif