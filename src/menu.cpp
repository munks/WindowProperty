#include "global.hpp"

//Variable

HMENU me_menu;

MENUITEMINFO me_mi = {sizeof(MENUITEMINFO), 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
NOTIFYICONDATA me_nid = {};
int me_infoCnt = 0;
UINT_PTR me_timer = 0;

//Internal

void Menu_SetStartup (BOOL add) {
	HKEY tmpkey;
	wchar_t path[MAX_PATH];
	wchar_t regval[MAX_PATH];
	
	GetModuleFileName(NULL, path, MAX_PATH);
	swprintf(regval, L"\"%s\" -hide", path);
	
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tmpkey, NULL);
	if (!add) {
		RegDeleteValue(tmpkey, L"WindowProperty");
	} else {
		RegSetValueEx(tmpkey, L"WindowProperty", 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
	}
	RegCloseKey(tmpkey);
}

void Menu_SetRTContext (BOOL add) {
	HKEY tmpkey;
	HKEY tmpkey2;
	wchar_t path[MAX_PATH];
	wchar_t regval[MAX_PATH];
	
	GetModuleFileName(NULL, path, MAX_PATH);
	
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\exefile\\shell\\WindowPropertyRTChecker", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tmpkey, NULL);
	if (!add) {
		RegDeleteTree(tmpkey, NULL);
		RegCloseKey(tmpkey);
		RegDeleteKey(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\exefile\\shell\\WindowPropertyRTChecker");
	} else {
		wcscpy(regval, BUTTON_RUNTIME_CAPTION);
		RegSetValueEx(tmpkey, NULL, 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		swprintf(regval, L"\"%ls\",0", path);
		RegSetValueEx(tmpkey, L"Icon", 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		RegCreateKeyEx(tmpkey, L"command", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tmpkey2, NULL);
		swprintf(regval, L"\"%ls\" -rtcheck \"%%1\"", path);
		RegSetValueEx(tmpkey2, NULL, 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		RegCloseKey(tmpkey);
		RegCloseKey(tmpkey2);
	}
}

void Menu_DeleteNotifyIcon (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	//Remove Info
	ZeroMemory(me_nid.szInfo, sizeof(me_nid.szInfo));
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	KillTimer(NULL, me_timer);
	me_timer = 0;
}

//External

void Menu_SetMenuState (WORD message, BOOL on) {
	me_mi.fMask = MIIM_STATE;
	GetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
	if (on) {
		me_mi.fState |= MFS_CHECKED;
	} else {
		me_mi.fState &= ~MFS_CHECKED;
	}
	SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
}

void Menu_SetMenuString (WORD message, LPWSTR str) {
	me_mi.fMask = MIIM_STRING;
	me_mi.dwTypeData = str;
	SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
}

void Menu_ExecuteNotifyEvent (WORD message) {
	RECT rect;
	BOOL changed;
	
	switch (message) {
		case TN_MENU_CLOSE: {
			Main_Close();
			return;
		}
		case TN_MENU_LOG: {
			if(!IsWindowVisible(l_window)) {
				//Set Log Window Position
				GetWindowRect(m_main, &rect);
				SetWindowPos(l_window, HWND_TOPMOST, rect.left + 20, rect.top + 20, 0, 0, SWP_NOSIZE);
			}
			//Show Log Window
			ShowWindow(l_window, SW_SHOW);
			break;
		}
		case TN_MENU_MOVE:
		case TN_MENU_CLIP:
		case TN_MENU_INIT:
		case TN_MENU_RT: {
			me_mi.fMask = MIIM_STATE;
			GetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
			me_mi.fState ^= MFS_CHECKED;
			SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
			changed = ((me_mi.fState & MFS_CHECKED) == MFS_CHECKED) ? BST_CHECKED : BST_UNCHECKED;
			switch (message) {
				case TN_MENU_MOVE:
					Hook_MoveHotkeyRegister(changed);
					break;
				case TN_MENU_CLIP:
					Hook_ClipHotkeyRegister(changed);
					break;
				case TN_MENU_INIT:
					Menu_SetStartup(changed);
					break;
				case TN_MENU_RT:
					Menu_SetRTContext(changed);
					break;
			}
			break;
		}
	}
}

void Menu_AddNotifyIcon () {
	me_nid.cbSize = sizeof(NOTIFYICONDATA);
	me_nid.hWnd = m_main;
	me_nid.uID = 0;
	me_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	me_nid.uCallbackMessage = WM_ICONNOTIFY;
	wcscpy(me_nid.szTip, WINDOW_MAIN_CAPTION);
	me_nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Shell_NotifyIcon(NIM_ADD, &me_nid);
}

void Menu_RemoveNotifyIcon () {
	me_nid.hWnd = m_main;
	me_nid.uID = 0;
	Shell_NotifyIcon(NIM_DELETE, &me_nid);
}

void Menu_InfoNotifyIcon (LPCWSTR title, LPCWSTR info, int len) {
	//Log
	Log_Message(LOG_FORMAT_NORMAL, title, info, NULL);
	
	//Init
	me_infoCnt++;
	me_nid.uFlags = NIF_INFO;
	me_nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	
	//Add Info
	wcscpy(me_nid.szInfoTitle, title);
	wcscpy(me_nid.szInfo, info);
	
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	//Wait
	if (me_timer) {
		KillTimer(NULL, me_timer);
	}
	me_timer = SetTimer(NULL, NOTIFY_DELETE, len, Menu_DeleteNotifyIcon);
}

void Menu_MakeMenu () {
	me_menu = CreatePopupMenu();
	
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_MOVE), MENU_MOVE_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLIP), MENU_CLIP_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_INIT), MENU_START_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_RT), MENU_RUNTIME_TEXT);
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_LOG), MENU_LOG_TEXT);
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLOSE), MENU_CLOSE_TEXT);
}