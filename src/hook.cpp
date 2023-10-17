#include "global.hpp"

//Variable

HWND h_window;
int h_moveHotkey = 0;
int h_clipHotkey = 0;

HHOOK h_hookMove = 0;
HHOOK h_hkHook = 0;
DWORD h_hkId;
BOOL h_cc = FALSE;
RECT h_ccRect[2];
POINT h_handlePoint;
POINT h_mousePoint;
HWND h_moveHandle = 0;

//Internal

LRESULT CALLBACK HotkeyChange (int nCode, WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			Util_SetHotkey(h_hkId, ((LPKBDLLHOOKSTRUCT)lParam)->vkCode);
			UnhookWindowsHookEx(h_hkHook);
			h_hkHook = 0;
			return 1;
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK HKProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WindowEventCase(uMsg) {
		WindowEvent(WM_SYSCOMMAND) {
			if (wParam == SC_CLOSE) {
				ShowWindow(hwnd, SW_HIDE);
				return 0;
			}
			break;
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_BUTTON_HK_MA)
				DialogEvent(ID_BUTTON_HK_CC) {
					if (EventMessage() == BN_CLICKED) {
						if (h_hkHook == 0) {
							switch (EventDialog()) {
								case ID_BUTTON_HK_MA: h_hkId = HOTKEY_MOVE; break;
								case ID_BUTTON_HK_CC: h_hkId = HOTKEY_CURSOR; break;
							}
							h_hkHook = SetWindowsHookEx(WH_KEYBOARD_LL, HotkeyChange, NULL, 0);
						}
					}
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

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

//External

void Hook_CreateWindow (HWND main) {
	HWND tmp;
	
	//Create Window Class (Hook)
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = HKProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = WINDOW_HOTKEY_NAME;
	wc.hbrBackground = m_hbrush;
	wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));

	Util_CheckError((void*)MAKELONG(RegisterClassEx(&wc), 0));
	
	//Create Log Window
	h_window = CreateWindowEx(WS_EX_TOPMOST, WINDOW_HOTKEY_NAME, WINDOW_HOTKEY_CAPTION,
							  WS_POPUP | WS_CAPTION | WS_SYSMENU,
							  CW_USEDEFAULT, CW_USEDEFAULT, 170, 150,
							  main, NULL, m_hInstance, NULL);
	Util_CheckError(l_window);
	
	tmp = CreateWindow(L"STATIC", BUTTON_MOVE_CAPTION,
						WS_CHILD | WS_VISIBLE | SS_CENTER,
						5, 10, 100, 20,
						h_window, NULL, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
	
	tmp = CreateWindow(L"STATIC", L"",
						WS_BORDER | WS_CHILD | WS_VISIBLE | SS_CENTER,
						5, 30, 100, 20,
						h_window, (HMENU)ID_STATIC_HK_MA, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
	
	tmp = CreateWindow(L"BUTTON", CHANGE_TEXT,
						WS_BORDER | WS_CHILD | WS_VISIBLE |
						BS_CENTER | BS_VCENTER,
						110, 30, 50, 20,
						h_window, (HMENU)ID_BUTTON_HK_MA, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
	
	tmp = CreateWindow(L"STATIC", BUTTON_CLIP_CAPTION,
						WS_CHILD | WS_VISIBLE | SS_CENTER,
						5, 60, 100, 20,
						h_window, NULL, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
	
	tmp = CreateWindow(L"STATIC", L"",
						WS_BORDER | WS_CHILD | WS_VISIBLE | SS_CENTER,
						5, 80, 100, 20,
						h_window, (HMENU)ID_STATIC_HK_CC, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
	
	tmp = CreateWindow(L"BUTTON", CHANGE_TEXT,
						WS_BORDER | WS_CHILD | WS_VISIBLE |
						BS_CENTER | BS_VCENTER,
						110, 80, 50, 20,
						h_window, (HMENU)ID_BUTTON_HK_CC, m_hInstance, NULL);
	Util_CheckError(tmp);
	SetWindowFont(tmp, m_font, FALSE);
}

void Hook_MoveHotkeyRegister (BOOL reg) {
	wchar_t txt[30];
	DWORD size = 4;

	//Unhook
	if (h_hookMove) { UnhookWindowsHookEx(h_hookMove); }
	
	if (reg) {
		//Get hotkey
		h_moveHotkey = Util_GetHotkey(HOTKEY_MOVE);
		
		//If hotkey is none (F3)
		if (h_moveHotkey == 0) {
			h_moveHotkey = VK_F3;
			Util_SetHotkey(HOTKEY_MOVE, h_moveHotkey);
		}
		
		//Register hotkey
		if (!RegisterHotKey(m_main, h_moveHotkey, MOD_NOREPEAT, h_moveHotkey)) {
			//If hotkey register failed
			MessageBoxA(m_main, "Failed To Register Hotkey", "Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			reg = FALSE;
		}
	} else {
		//Unregister hotkey
		UnregisterHotKey(m_main, h_moveHotkey);
	}
	
	//Button/Menu Icon/Registry set
	Button_SetCheck(GetDlgItem(m_main, ID_BUTTON_MOVE), reg);
	if (reg) {
		swprintf(txt, MENU_MOVE_TEXT_FORMAT, VirtualKeyCodeText(h_moveHotkey));
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

	if (reg) {
		//Get hotkey
		h_clipHotkey = Util_GetHotkey(HOTKEY_CURSOR);
		
		//If hotkey is none (F4)
		if (h_clipHotkey == 0) {
			h_clipHotkey = VK_F4;
			Util_SetHotkey(HOTKEY_CURSOR, h_clipHotkey);
		}
		
		//Register hotkey
		if (!RegisterHotKey(m_main, h_clipHotkey, MOD_NOREPEAT, h_clipHotkey)) {
			//If hotkey register failed
			MessageBoxA(m_main, "Failed To Register Hotkey", "Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			reg = FALSE;
		}
	} else {
		//Unregister hotkey
		UnregisterHotKey(m_main, h_clipHotkey);
	}
	
	//Button/Menu Icon/Registry set
	Button_SetCheck(GetDlgItem(m_main, ID_BUTTON_CLIP), reg);
	if (reg) {
		swprintf(txt, MENU_CLIP_TEXT_FORMAT, VirtualKeyCodeText(h_clipHotkey));
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
	
	GetClipCursor(&tmpRect);
	
	if (tmpRect != h_ccRect[1]) {
		GetClipCursor(&h_ccRect[0]);
		GetWindowRect(GetForegroundWindow(), &h_ccRect[1]);
		if (ClipCursor(&h_ccRect[1])) {
			Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_ACTIVE, 3000);
		} else {
			Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_FAILED, 3000);
		}
	} else {
		ClipCursor(&h_ccRect[0]);
		Menu_InfoNotifyIcon(NOTIFY_HOTKEY, NOTIFY_CLIP_DEACTIVE, 3000);
	}
}