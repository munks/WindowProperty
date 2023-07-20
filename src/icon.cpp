#include "global.hpp"

//Variable

GUID i_guid = {0x8DAD0D50, 0x1DCF, 0x4233, {0xB6, 0x70, 0x28, 0x43, 0x8D, 0x33, 0x4D, 0x9E}};
HMENU i_menu;

#define AppendMenuString(menu, option, idtf, str) AppendMenu(menu, MF_STRING | option, MAKELONG(ID_BUTTON_ICON, idtf), str)
#define AppendMenuSeparator(menu) AppendMenu(menu, MF_SEPARATOR, 0, NULL)

//External

void Icon_ExecuteNotifyEvent (WORD message) {
	RECT rect;
	wchar_t menuStr[30];
	MENUITEMINFO mi = {0};
	
	switch (message) {
		case TN_MENU_CLOSE: {
			DeleteObject(m_font);
			RegCloseKey(m_regkey);
			Icon_RemoveNotifyIcon();
			FreeLibrary(c_comctlModule);
			PostQuitMessage(0);
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
		case TN_MENU_OPTION1:
		case TN_MENU_OPTION2:
		case TN_MENU_OPTION3: {
			//Toggle The Triggered Option
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_STRING | MIIM_STATE;
			mi.dwTypeData = menuStr;
			mi.cch = 30;
			
			GetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, message), false, &mi);
			mi.fState ^= MFS_CHECKED;
			SetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, message), false, &mi);
			break;
		}
	}
}

void Icon_AddNotifyIcon (HWND main) {
	NOTIFYICONDATA nid = {0, };
	
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = main;
	nid.guidItem = i_guid;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID;
	nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));
	nid.uCallbackMessage = WM_ICONNOTIFY;
	wcscpy(nid.szTip, WINDOW_MAIN_NAME);
	
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void Icon_RemoveNotifyIcon () {
	NOTIFYICONDATA nid = {0, };
	
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_GUID;
	nid.guidItem = i_guid;
	
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void Icon_MakeMenu () {
	i_menu = CreatePopupMenu();
	
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_OPTION1, L"Dummy Option1");
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_OPTION2, L"Dummy Option2");
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_OPTION3, L"Dummy Option3");
	AppendMenuSeparator(i_menu);
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_LOG, L"Log");
	AppendMenuSeparator(i_menu);
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_CLOSE, L"Close");
}