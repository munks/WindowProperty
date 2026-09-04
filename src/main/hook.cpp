#include "global.hpp"

//Variable

HHOOK h_hookMove = 0;
HANDLE h_hookClip = 0;

POINT h_handlePoint;
POINT h_mousePoint;
HWND h_moveHandle = 0;

//Internal

LRESULT CALLBACK MoveHook (int nCode, WPARAM wParam, LPARAM lParam) {
	RECT rect;
	POINT point;
	
	switch (wParam) {
		case WM_LBUTTONDOWN: {
			h_mousePoint = ((LPMSLLHOOKSTRUCT)lParam)->pt;
			h_moveHandle = GetAncestor(WindowFromPoint(h_mousePoint), GA_ROOTOWNER);
			GetWindowRect(h_moveHandle, &rect);
			h_handlePoint = {rect.left, rect.top};
			return 1;
		}
		case WM_LBUTTONUP: {
			h_moveHandle = 0;
			return 1;
		}
		case WM_MOUSEMOVE: {
			if (h_moveHandle) {
				point = ((LPMSLLHOOKSTRUCT)lParam)->pt;
				SetWindowPos(h_moveHandle, HWND_TOP, h_handlePoint.x - (h_mousePoint.x - point.x), h_handlePoint.y - (h_mousePoint.y - point.y),
							0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
				h_mousePoint = point;
				GetWindowRect(h_moveHandle, &rect);
				h_handlePoint = {rect.left, rect.top};
			}
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI ClipCheck (LPVOID param) {
	RECT rect;
	
	while (h_hookClip) {
		GetWindowRect((HWND)param, &rect);
		ClipCursor(&rect);
		Sleep(1);
	}
	ClipCursor(NULL);
	
	return 0;
}

//External

void Hook_MoveHotkeyRegister (BOOL reg) {
	wchar_t txt[30];
	DWORD size = 4;

	//Unhook
	if (h_hookMove) {
		UnhookWindowsHookEx(h_hookMove);
		h_hookMove = 0;
	}
	
	if (reg) {
		//If hotkey is none (F3)
		if (Util_GetHotkey(HOTKEY_MOVE, HK_TYPE_ALL) == 0) {
			Util_SetHotkey(HOTKEY_MOVE, VK_F3);
		}
		
		//Register hotkey
		if (!RegisterHotKey(m_main, HOTKEY_MOVE, MOD_NOREPEAT | Util_GetHotkey(HOTKEY_MOVE, HK_TYPE_AK), Util_GetHotkey(HOTKEY_MOVE, HK_TYPE_VK))) {
			//If hotkey register failed
			MessageBoxA(m_main, "Failed To Register Hotkey", "Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			reg = FALSE;
		}
	} else {
		//Unregister hotkey
		UnregisterHotKey(m_main, HOTKEY_MOVE);
	}
	
	//Button/Menu Icon/Registry set
	Button_SetCheck(GetDlgItem(m_main, ID_BUTTON_MOVE), reg);
	if (reg) {
		swprintf(txt, MENU_MOVE_TEXT_FORMAT, Util_GetHotkeyString(HOTKEY_MOVE));
	} else {
		wcscpy(txt, MENU_MOVE_TEXT);
	}
	Menu_SetMenuString(TN_MENU_MOVE, txt);
	Menu_SetMenuState(TN_MENU_MOVE, reg);
	RegSetValueEx(m_regkey, L"MoveActive", 0, REG_BINARY, (BYTE*)&reg, sizeof(BYTE));
	
	#ifdef _DEBUG
	printf("Move Hotkey %s\n", reg ? "Registered" : "Unregistered");
	#endif
}

void Hook_ClipHotkeyRegister (BOOL reg) {
	wchar_t txt[30];
	DWORD size = 4;
	
	//Terminate
	if (h_hookClip) {
		h_hookClip = 0;
	}
	
	if (reg) {
		//If hotkey is none (F4)
		if (Util_GetHotkey(HOTKEY_CURSOR, HK_TYPE_ALL) == 0) {
			Util_SetHotkey(HOTKEY_CURSOR, VK_F4);
		}
		
		//Register hotkey
		if (!RegisterHotKey(m_main, HOTKEY_CURSOR, MOD_NOREPEAT | Util_GetHotkey(HOTKEY_CURSOR, HK_TYPE_AK), Util_GetHotkey(HOTKEY_CURSOR, HK_TYPE_VK))) {
			//If hotkey register failed
			MessageBoxA(m_main, "Failed To Register Hotkey", "Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			reg = FALSE;
		}
	} else {
		//Unregister hotkey
		UnregisterHotKey(m_main, HOTKEY_CURSOR);
	}
	
	//Button/Menu Icon/Registry set
	Button_SetCheck(GetDlgItem(m_main, ID_BUTTON_CLIP), reg);
	if (reg) {
		swprintf(txt, MENU_CLIP_TEXT_FORMAT, Util_GetHotkeyString(HOTKEY_CURSOR));
	} else {
		wcscpy(txt, MENU_CLIP_TEXT);
	}
	Menu_SetMenuString(TN_MENU_CLIP, txt);
	Menu_SetMenuState(TN_MENU_CLIP, reg);
	RegSetValueEx(m_regkey, L"CursorActive", 0, REG_BINARY, (BYTE*)&reg, sizeof(BYTE));
	
	#ifdef _DEBUG
	printf("Clip Cursor %s\n", reg ? "Registered" : "Unregistered");
	#endif
}

void Hook_MouseHook () {
	if (!h_hookMove) {
		h_hookMove = SetWindowsHookEx(WH_MOUSE_LL, MoveHook, NULL, 0);
		Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_MOVE_ACTIVE, 3000);
	} else {
		UnhookWindowsHookEx(h_hookMove);
		h_hookMove = 0;
		Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_MOVE_DEACTIVE, 3000);
	}
	
	#ifdef _DEBUG
	printf("Mouse Hook %s\n", h_hookMove ? "Attached" : "Detached");
	#endif
}

void Hook_ClipCursor () {
	RECT tmpRect;
	
	if (!h_hookClip) {
		GetWindowRect(GetForegroundWindow(), &tmpRect);
		if (ClipCursor(&tmpRect)) {
			h_hookClip = CreateThread(NULL, 0, ClipCheck, (LPVOID)GetForegroundWindow(), 0, NULL);
			Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_ACTIVE, 3000);
		} else {
			Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_FAILED, 3000);
		}
	} else {
		h_hookClip = 0;
		Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_DEACTIVE, 3000);
	}
	
	#ifdef _DEBUG
	printf("Cursor Hook %s\n", h_hookClip ? "Attached" : "Detached");
	#endif
}