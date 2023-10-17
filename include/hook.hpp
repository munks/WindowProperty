#pragma once

#define HOTKEY_MOVE		1
#define HOTKEY_CURSOR	2

void Hook_CreateWindow (HWND);
void Hook_MoveHotkeyRegister (BOOL);
void Hook_ClipHotkeyRegister (BOOL);
void Hook_MouseHook ();
void Hook_ClipCursor ();

extern HWND h_window;
extern int h_moveHotkey;
extern int h_clipHotkey;