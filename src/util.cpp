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

wchar_t* Util_GetHotkeyString (DWORD hotkey) {
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

void Util_SetHotkeyText (DWORD hotkey, DWORD vk) {
	switch (hotkey) {
		case HOTKEY_MOVE:
			SetWindowText(GetDlgItem(h_window, ID_STATIC_HK_MA), VirtualKeyCodeText(vk));
			break;
		case HOTKEY_CURSOR:
			SetWindowText(GetDlgItem(h_window, ID_STATIC_HK_CC), VirtualKeyCodeText(vk));
			break;
	}
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

DWORD Util_GetHotkey (DWORD hotkey) {
	DWORD tmp, rtn;
	DWORD size = 4;
	LSTATUS result;
	
	result = RegGetValue(m_regkey, NULL, Util_GetHotkeyString(hotkey), RRF_RT_DWORD, NULL, &tmp, &size);
	rtn = result ? 0 : tmp;
	
	Util_SetHotkeyText(hotkey, rtn);
	
	return rtn;
}

LSTATUS Util_SetHotkey (DWORD hotkey, DWORD vk) {
	wchar_t* keystr = Util_GetHotkeyString(hotkey);
	
	if (keystr == NULL) {
		return ERROR_INVALID_DATA;
	}
	
	Util_SetHotkeyText(hotkey, vk);
	
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

void Util_SettingConfig (LONG_PTR* prop, HWND hwnd, bool add) {
	wchar_t str[256], *exe;
	DWORD len;
	HKEY key;
	HANDLE handle;
	
	handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Util_GetProcessID(hwnd));
    QueryFullProcessImageName(handle, 0, str, &(len = 256));
    CloseHandle(handle);
	
	exe = wcsrchr(str, L'\\') + 1;
	GetClassName(hwnd, str, 256);
	
	RegCreateKeyEx(m_regset, exe, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, NULL);
	
	if (add) {
		RegSetValueEx(key, L"Style", 0, REG_DWORD, (BYTE*)&prop[0], sizeof(DWORD));
		RegSetValueEx(key, L"ExStyle", 0, REG_DWORD, (BYTE*)&prop[1], sizeof(DWORD));
		RegSetValueEx(key, L"Class", 0, REG_SZ, (BYTE*)&str, wcslen(str) * sizeof(wchar_t));
	} else {
		RegDeleteTree(m_regset, exe);
		RegDeleteKey(m_regset, exe);
	}
	
	RegCloseKey(key);
	
	while (m_loopStop) {
		Sleep(500);
	}
	m_loopStop = true;
}

void Util_PrintInt (int i) {
	char txt[260];
	
	sprintf(txt, "%d", i);
	MessageBoxA(NULL, txt, "Num.", MB_OK);
}

void Util_PrintString (const wchar_t* str) {
	MessageBox(NULL, str, L"Str.", MB_OK);
}