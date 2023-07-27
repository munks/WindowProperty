#include "global.hpp"

//Internal

bool CheckErrorFunc (void* checkVar, LPCSTR file, int line, LPCSTR targetValName) {
	char errmsg[150];
	
	if (checkVar == NULL) {
		sprintf(errmsg, "Window Processing Error\nOn File - %s, In Line - %d\nVariable Name - %s, Error Code - %d", file, line, targetValName, GetLastError());
		MessageBoxA(NULL, errmsg, "Error", MB_OK | MB_ICONERROR);
		Main_Close();
	}
	
	return true;
}

//External

ULONG Util_GetProcessID (HWND hwnd) {
	ULONG idProc;
	GetWindowThreadProcessId(hwnd, &idProc);
	return idProc;
}

bool Util_IsWindowShown (HWND hwnd) {
	int style = GetWindowStyle(hwnd);
	int exstyle = GetWindowExStyle(hwnd);

	if (!Button_GetCheck(m_allButton)) {
		if (!(style & WS_VISIBLE)) {
			return false;
		}
	}
	if (exstyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
		return false;
	}
	return true;
}