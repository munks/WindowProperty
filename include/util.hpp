#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILE__, __LINE__, #t)
bool CheckErrorFunc (void*, LPCSTR, int, LPCSTR);
ULONG Util_GetProcessID (HWND);
bool Util_IsWindowShown (HWND);
DWORD Util_GetHotkey (DWORD);
LSTATUS Util_SetHotkey (DWORD, DWORD);
bool operator==(RECT, RECT);
bool operator!=(RECT, RECT);