#pragma once

void Process_WindowPropChange (HWND, HWND, LPCWSTR);
void Process_WindowCaptionChange (HWND, HWND, LPCWSTR);
void Process_WindowOpacityChange (HWND, HWND, LPCWSTR);
void Process_WindowFullScreenChange (HWND, HWND, LPCWSTR);
void Process_WindowsDLLHook (HWND, HWND, LPCWSTR);
void Process_EnumModule (HWND, HWND, LPCWSTR);
void Process_OpenDirectory (HWND, HWND, LPCWSTR);
void Process_ChangeHotkey (HWND, HWND, LPCWSTR);
void Process_ChangeFilter (HWND, HWND, LPCWSTR);

#define TYPE_DLG_PROP 1
#define TYPE_DLG_FILTER 2