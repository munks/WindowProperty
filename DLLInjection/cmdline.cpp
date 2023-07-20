#include <wchar.h>
#include <windows.h>

static DWORD WINAPI ThreadProc (LPVOID lparam) {
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
	FreeLibraryAndExitThread((HMODULE)lparam, 0);
	return 0;
}

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH: {
			CreateThread(NULL, 0, ThreadProc, (void*)hinstDLL, 0, NULL);
			break;
		}
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
		case DLL_PROCESS_DETACH: break;
	}

	return true;
}