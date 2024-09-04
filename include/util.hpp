#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILE__, __LINE__, #t)
#define Util_PrintWindowsLastError() Util_PrintWindowsLastErrorInternal(__FILE__, __FUNCTION__, __LINE__)
bool CheckErrorFunc (void*, LPCSTR, int, LPCSTR);
ULONG Util_GetProcessID (HWND);
bool Util_WindowFilter (HWND);
DWORD Util_GetHotkey (DWORD, int);
LPWSTR Util_GetHotkeyString (DWORD);
LSTATUS Util_SetHotkey (DWORD, DWORD);
void Util_PrintWindowsLastErrorInternal (LPCSTR, LPCSTR, int);
bool operator==(RECT, RECT);
bool operator!=(RECT, RECT);
void Util_PrintInt (int);
void Util_PrintString (const wchar_t*);
DWORD Util_GetWDAState (HWND);
void Util_DateOperate (FILETIME*, FILETIME*);
void Util_FileTimeToTime (FILETIME*, SYSTEMTIME*);

extern LONG_PTR u_filter[2][2];

#define HK_TYPE_VK		(1 << 0)
#define HK_TYPE_AK		(1 << 1)
#define HK_TYPE_ALL		HK_TYPE_VK | HK_TYPE_AK