#include "global.hpp"

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

wchar_t* GetHotkeyString (DWORD hotkey) {
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

void SetHotkeyText (DWORD hotkey, DWORD vk) {
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

bool Util_IsWindowShown (HWND hwnd) {
	int style = GetWindowStyle(hwnd);
	int exstyle = GetWindowExStyle(hwnd);

	if (!Button_GetCheck(GetDlgItem(m_main, ID_BUTTON_ALL))) {
		if (!(style & WS_VISIBLE)) {
			return false;
		}
	}
	if (exstyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
		return false;
	}
	
	return true;
}

DWORD Util_GetHotkey (DWORD hotkey) {
	DWORD tmp, rtn;
	DWORD size = 4;
	LSTATUS result;
	
	result = RegGetValue(m_regkey, NULL, GetHotkeyString(hotkey), RRF_RT_DWORD, NULL, &tmp, &size);
	rtn = result ? 0 : tmp;
	
	SetHotkeyText(hotkey, rtn);
	
	return rtn;
}

LSTATUS Util_SetHotkey (DWORD hotkey, DWORD vk) {
	wchar_t* keystr = GetHotkeyString(hotkey);
	
	if (keystr == NULL) {
		return ERROR_INVALID_DATA;
	}
	
	SetHotkeyText(hotkey, vk);
	
	return RegSetValueEx(m_regkey, keystr, 0, REG_DWORD, (BYTE*)&vk, sizeof(DWORD));
}