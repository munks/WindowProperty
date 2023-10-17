#pragma once

typedef struct INFODATA {
	LPCWSTR title;
	LPCWSTR info;
	int len;
} INFODATA;

void Menu_SetMenuString (WORD, LPWSTR);
void Menu_SetMenuState (WORD, BOOL);
void Menu_ExecuteNotifyEvent (WORD);
void Menu_AddNotifyIcon (HWND);
void Menu_RemoveNotifyIcon ();
void Menu_MakeMenu ();
void Menu_InfoNotifyIcon (LPCWSTR, LPCWSTR, int);

extern HMENU me_menu;