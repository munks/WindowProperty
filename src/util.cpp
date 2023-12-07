#include "global.hpp"

//Variable

LONG_PTR u_filter[2][2] = {{WS_VISIBLE, 0}, {0, WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE}};

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

wchar_t* Util_GetHotkeyRegkey (DWORD hotkey) {
	static wchar_t str[20];
	
	switch (hotkey) {
		case HOTKEY_MOVE:
			wcscpy(str, L"MoveActiveKey");
			break;
		case HOTKEY_CURSOR:
			wcscpy(str, L"CursorActiveKey");
			break;
		default:
			return NULL;
	}
	
	return str;
}

//External

bool operator==(RECT src, RECT cmp) {
	if (src.left != cmp.left) {
		return false;
	}
	if (src.top != cmp.top) {
		return false;
	}
	if (src.right != cmp.right) {
		return false;
	}
	if (src.bottom != cmp.bottom) {
		return false;
	}
	return true;
}

bool operator!=(RECT src, RECT cmp) {
	if (src.left != cmp.left) {
		return true;
	}
	if (src.top != cmp.top) {
		return true;
	}
	if (src.right != cmp.right) {
		return true;
	}
	if (src.bottom != cmp.bottom) {
		return true;
	}
	return false;
}

ULONG Util_GetProcessID (HWND hwnd) {
	ULONG idProc;
	
	GetWindowThreadProcessId(hwnd, &idProc);
	
	return idProc;
}

bool Util_WindowFilter (HWND hwnd) {
	int style = GetWindowStyle(hwnd);
	int exstyle = GetWindowExStyle(hwnd);
	
	//Include
	if ((style & u_filter[0][0]) != u_filter[0][0]) {
		return false;
	}
	if ((exstyle & u_filter[1][0]) != u_filter[1][0]) {
		return false;
	}
	//Exclude
	if (style & u_filter[0][1]) {
		return false;
	}
	if (exstyle & u_filter[1][1]) {
		return false;
	}
	
	return true;
}

DWORD Util_GetHotkey (DWORD hotkey, int type) {
	DWORD tmp, rtn;
	DWORD size;
	LSTATUS result;
	WORD data;
	BYTE vk, ak;
	
	result = RegGetValue(m_regkey, NULL, Util_GetHotkeyRegkey(hotkey), RRF_RT_DWORD, NULL, &tmp, &(size = 4));
	
	if (result) {
		return 0;
	}
	
	data = LOWORD(tmp);
	vk = LOBYTE(data);
	ak = HIBYTE(data);
	
	switch (type) {
		case HK_TYPE_VK:
			return vk;
		case HK_TYPE_AK:
			rtn = 0;
			if (ak & HOTKEYF_ALT) { rtn |= MOD_ALT; }
			if (ak & HOTKEYF_CONTROL) { rtn |= MOD_CONTROL; }
			if (ak & HOTKEYF_SHIFT) { rtn |= MOD_SHIFT; }
			return rtn;
		case HK_TYPE_ALL:
			return tmp;
	}

	return 0;
}

LPWSTR Util_GetHotkeyString (DWORD hotkey) {
	static wchar_t out[30];
	DWORD tmp;
	DWORD size;
	LSTATUS result;
	WORD data;
	BYTE vk, ak;
	
	result = RegGetValue(m_regkey, NULL, Util_GetHotkeyRegkey(hotkey), RRF_RT_DWORD, NULL, &tmp, &(size = 4));
	
	if (result) {
		wcscpy(out, VK_NONE);
		return out;
	}
	
	data = LOWORD(tmp);
	vk = LOBYTE(data);
	ak = HIBYTE(data);
	
	out[0] = L'\0';
	if (ak & HOTKEYF_CONTROL) { wcscat(out, L"C"); }
	if (ak & HOTKEYF_SHIFT) { wcscat(out, L"S"); }
	if (ak & HOTKEYF_ALT) { wcscat(out, L"A"); }
	if (ak) { wcscat(out, L" + "); }
	wcscat(out, VirtualKeyCodeText(vk));
	
	return out;
}

LSTATUS Util_SetHotkey (DWORD hotkey, DWORD vk) {
	wchar_t* keystr = Util_GetHotkeyRegkey(hotkey);
	
	if (keystr == NULL) {
		return ERROR_INVALID_DATA;
	}
	
	return RegSetValueEx(m_regkey, keystr, 0, REG_DWORD, (BYTE*)&vk, sizeof(DWORD));
}

void Util_PrintWindowsLastError () {
	LPWSTR msg;
	
	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPWSTR)&msg, 0, NULL);
	
	MessageBox(NULL, msg, L"GetLastError", MB_OK);
	
	LocalFree(msg);
}

void Util_PrintInt (int i) {
	char txt[260];
	
	sprintf(txt, "%d", i);
	MessageBoxA(NULL, txt, "Num.", MB_OK);
}

void Util_PrintString (const wchar_t* str) {
	MessageBox(NULL, str, L"Str.", MB_OK);
}

DWORD Util_GetWDAState (HWND hwnd) {
	DWORD wda;
	
	GetWindowDisplayAffinity(hwnd, &wda);
	
	return wda;
}
