#pragma once

#define Util_CheckError(t) CheckErrorFunc(t, __FILE__, __LINE__, #t)
template<typename T>
bool CheckErrorFunc(T checkVar, LPCSTR file, int line, LPCSTR targetValName) {
	if (checkVar == NULL) {
		std::string errmsg = std::format(
			"Window Processing Error\n"
			"On File - {}\nIn Line - {}\n"
			"Variable Name - {}\nError Code - {:#x}",
			file, line, targetValName, GetLastError());
		MessageBoxA(nullptr, errmsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}

	return true;
}
#define Util_PrintWindowsLastError() Util_PrintWindowsLastErrorInternal(__FILEW__, __FUNCTIONW__, __LINE__)
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