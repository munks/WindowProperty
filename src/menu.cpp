#include "global.hpp"

//Variable

HMENU me_menu;

GUID me_guid = {0x8DAD0D50, 0x1DCF, 0x4233, {0xB6, 0x70, 0x28, 0x43, 0x8D, 0x33, 0x4D, 0x9E}};
MENUITEMINFO me_mi = {sizeof(MENUITEMINFO), 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
NOTIFYICONDATA me_nid = {sizeof(NOTIFYICONDATA), NULL, 0, 0, WM_ICONNOTIFY, NULL, WINDOW_MAIN_NAME,
						0, 0, {0}, 0, {0}, 0, me_guid, NULL};
int me_infoCnt = 0;
INFODATA me_param;

//Internal

void SetStartup (BOOL add) {
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

DWORD WINAPI InfoNotifyIcon_Internal (LPVOID param) {
	//Init
	me_infoCnt++;
	me_nid.uFlags = NIF_INFO | NIF_GUID;
	me_nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	
	//Remove Info
	ZeroMemory(me_nid.szInfo, sizeof(me_nid.szInfo));
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	//Add Info
	wcscpy(me_nid.szInfoTitle, me_param.title);
	wcscpy(me_nid.szInfo, me_param.info);
	
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	//Wait
	Sleep(me_param.len);
	
	//Remove Info (If there is no other info)
	me_infoCnt--;
	if (!me_infoCnt) {
		ZeroMemory(me_nid.szInfo, sizeof(me_nid.szInfo));
		Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	}
	return 0;
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
		case TN_MENU_INIT: {
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
					SetStartup(changed);
					break;
			}
			break;
		}
	}
}

void Menu_AddNotifyIcon (HWND main) {
	me_nid.hWnd = main;
	me_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID;
	me_nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Shell_NotifyIcon(NIM_ADD, &me_nid);
}

void Menu_RemoveNotifyIcon () {
	me_nid.uFlags = NIF_GUID;
	
	Shell_NotifyIcon(NIM_DELETE, &me_nid);
}

void Menu_InfoNotifyIcon (LPCWSTR title, LPCWSTR info, int len) {
	me_param.title = title;
	me_param.info = info;
	me_param.len = len;
	CreateThread(NULL, 0, InfoNotifyIcon_Internal, NULL, 0, NULL);
}

void Menu_MakeMenu () {
	me_menu = CreatePopupMenu();
	
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_MOVE), L"Move Active");
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLIP), L"Clip Cursor");
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_INIT), L"Add Startup");
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_LOG), L"Log");
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLOSE), L"Close");
}