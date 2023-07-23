#include "global.hpp"

//Variable

GUID i_guid = {0x8DAD0D50, 0x1DCF, 0x4233, {0xB6, 0x70, 0x28, 0x43, 0x8D, 0x33, 0x4D, 0x9E}};
HMENU i_menu;
MENUITEMINFO i_mi = {sizeof(MENUITEMINFO), MIIM_STATE, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
NOTIFYICONDATA i_nid = {sizeof(NOTIFYICONDATA), NULL, 0, 0, WM_ICONNOTIFY, NULL, WINDOW_MAIN_NAME,
						0, 0, {0}, 0, {0}, 0, i_guid, NULL};

#define AppendMenuString(menu, option, idtf, str) AppendMenu(menu, MF_STRING | option, MAKELONG(ID_BUTTON_ICON, idtf), str)
#define AppendMenuSeparator(menu) AppendMenu(menu, MF_SEPARATOR, 0, NULL)

//External

void Icon_SetIconState (WORD message, BOOL on) {
	GetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, message), false, &i_mi);
	if (on) {
		i_mi.fState |= MFS_CHECKED;
	} else {
		i_mi.fState &= ~MFS_CHECKED;
	}
	SetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, message), false, &i_mi);
}

void Icon_ExecuteNotifyEvent (WORD message) {
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
		case TN_MENU_MOVE: {
			GetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, TN_MENU_MOVE), false, &i_mi);
			i_mi.fState ^= MFS_CHECKED;
			SetMenuItemInfo(i_menu, MAKELONG(ID_BUTTON_ICON, TN_MENU_MOVE), false, &i_mi);
			changed = ((i_mi.fState & MFS_CHECKED) == MFS_CHECKED) ? BST_CHECKED : BST_UNCHECKED;
			Button_SetCheck(GetDlgItem(m_main, ID_BUTTON_MOVE), changed);
			changed ? Hook_MoveCallbackAttach() : Hook_MoveCallbackDetach();
			RegSetValueEx(m_regkey, L"MoveActive", 0, REG_BINARY, (BYTE*)&changed, sizeof(BYTE));
			break;
		}
	}
}

void Icon_AddNotifyIcon (HWND main) {
	i_nid.hWnd = main;
	i_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID;
	i_nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Shell_NotifyIcon(NIM_ADD, &i_nid);
}

void Icon_RemoveNotifyIcon () {
	i_nid.uFlags = NIF_GUID;
	
	Shell_NotifyIcon(NIM_DELETE, &i_nid);
}

void Icon_MakeMenu () {
	i_menu = CreatePopupMenu();
	
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_MOVE, L"Move Active");
	AppendMenuSeparator(i_menu);
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_LOG, L"Log");
	AppendMenuSeparator(i_menu);
	AppendMenuString(i_menu, MF_UNCHECKED, TN_MENU_CLOSE, L"Close");
}