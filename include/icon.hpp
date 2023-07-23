#pragma once

#define Icon_GetMenu() i_menu

void Icon_SetIconState (WORD, BOOL);
void Icon_ExecuteNotifyEvent (WORD);
void Icon_AddNotifyIcon (HWND);
void Icon_RemoveNotifyIcon ();
void Icon_MakeMenu ();

extern HMENU i_menu;