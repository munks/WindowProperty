#include <wchar.h>
#include <windows.h>

typedef long long HWND_T;

void CommandLine () {
	HGLOBAL cbm;
	LPVOID glm;
	
	if (!OpenClipboard(NULL)) { goto END; }
	EmptyClipboard();
	
	cbm = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * (wcslen(GetCommandLineW()) + 1));
	if (!cbm) { CloseClipboard(); goto END;}
	
	glm = GlobalLock(cbm);
	memcpy(glm, GetCommandLineW(), sizeof(wchar_t) * (wcslen(GetCommandLineW()) + 1));
	
	GlobalUnlock(cbm);
	
	SetClipboardData(CF_UNICODETEXT, cbm);
	
	CloseClipboard();
	
	END:
	MessageBoxW(NULL, GetCommandLineW(), L"Command Line", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
}

void CaptureAffinity (BOOL on) {
	DWORD size;
	HWND_T hwnd;
	
	size = sizeof(HWND_T);
	RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Duality\\WindowProperty", L"LastHookHWND", RRF_RT_REG_QWORD, NULL, (LPVOID)&hwnd, &size);
	
	SetWindowDisplayAffinity((HWND)hwnd, on ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
}

DWORD WINAPI TypeCheck (LPVOID lParam) {
	DWORD size;
	BYTE val;
	
	size = sizeof(BYTE);
	RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Duality\\WindowProperty", L"LastHook", RRF_RT_REG_BINARY, NULL, &val, &size);
	
	if (val & 0x1) {
		CaptureAffinity(val & 0x2);
	} else {
		CommandLine();
	}
	
	FreeLibraryAndExitThread((HMODULE)lParam, 0);
	return 0;
}


BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH: {
			CreateThread(NULL, 0, TypeCheck, (void*)hinstDLL, 0, NULL);
			break;
		}
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
		case DLL_PROCESS_DETACH: break;
	}

	return true;
}