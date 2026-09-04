#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILEW__, __LINE__, #t)
#define Util_PrintWindowsLastError() Util_PrintWindowsLastErrorInternal(__FILEW__, __FUNCTIONW__, __LINE__)
bool CheckErrorFunc (void*, LPCWSTR, int, LPCSTR);
ULONG Util_GetProcessID (HWND);
bool Util_WindowFilter (HWND);
DWORD Util_GetHotkey (DWORD, int);
LPWSTR Util_GetHotkeyString (DWORD);
LSTATUS Util_SetHotkey (DWORD, DWORD);
void Util_PrintWindowsLastErrorInternal (LPCWSTR, LPCWSTR, int);
bool operator==(RECT, RECT);
bool operator!=(RECT, RECT);
void Util_PrintInt (int);
void Util_PrintString (const wchar_t*);
DWORD Util_GetWDAState (HWND);
void Util_DateOperate (FILETIME*, FILETIME*);
void Util_FileTimeToTime (FILETIME*, SYSTEMTIME*);

extern LONG_PTR u_filter[2][2];

constexpr int HK_TYPE_VK	= (1 << 0);
constexpr int HK_TYPE_AK	= (1 << 1);
constexpr int HK_TYPE_ALL	= HK_TYPE_VK | HK_TYPE_AK;