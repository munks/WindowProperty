#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILE__, __LINE__, #t)
bool CheckErrorFunc (void*, LPCSTR, int, LPCSTR);
ULONG Util_GetProcessID (HWND);
bool Util_WindowFilter (HWND);
DWORD Util_GetHotkey (DWORD);
LSTATUS Util_SetHotkey (DWORD, DWORD);
void Util_PrintWindowsLastError ();
void Util_SettingConfig (LONG_PTR*, HWND, bool);
bool operator==(RECT, RECT);
bool operator!=(RECT, RECT);
void Util_PrintInt (int);
void Util_PrintString (const wchar_t*);

extern LONG_PTR u_filter[2][2];