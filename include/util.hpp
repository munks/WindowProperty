#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILE__, __LINE__, #t)
bool CheckErrorFunc (void*, LPCSTR, int, LPCSTR);
ULONG Util_GetProcessID (HWND);
bool Util_WindowFilter (HWND);
DWORD Util_GetHotkey (DWORD, int);
LPWSTR Util_GetHotkeyString (DWORD);
LSTATUS Util_SetHotkey (DWORD, DWORD);
void Util_PrintWindowsLastError ();
bool operator==(RECT, RECT);
bool operator!=(RECT, RECT);
void Util_PrintInt (int);
void Util_PrintString (const wchar_t*);
DWORD Util_GetWDAState (HWND);

extern LONG_PTR u_filter[2][2];

#define HK_TYPE_VK		(1 << 0)
#define HK_TYPE_AK		(1 << 1)
#define HK_TYPE_ALL		HK_TYPE_VK | HK_TYPE_AK