#pragma once

void DlgFunction_ResetFilter (HWND);
void DlgFunction_SetProperties (HWND, PLONG_PTR, PLONG_PTR);
void DlgFunction_SetFilter (HWND);
void DlgFunction_PropInit (HWND, LONG_PTR[], LONG_PTR[], bool, LPCWSTR, bool);
void DlgFunction_PropButtonState (HWND, int, HWND);
DWORD WINAPI DlgFunction_SystemTimeLoop (LPVOID);
bool DlgFunction_HotkeyValidCheck (HWND, int, int);

typedef struct TDATA {
	HWND hwnd;
	HWND timer;
	FILETIME ft;
} TDATA;