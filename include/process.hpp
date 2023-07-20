#pragma once

void Process_WindowTopmostStateChange (HWND, HWND, LPCWSTR);
void Process_WindowCaptionChange (HWND, HWND, LPCWSTR);
void Process_WindowOpacityChange (HWND, HWND, LPCWSTR);
void Process_WindowCaptionDetach (HWND, HWND, LPCWSTR);
void Process_WindowFullScreenChange (HWND, HWND, LPCWSTR);
void Process_WindowPIPChange (HWND, HWND, LPCWSTR);
void Process_CommandLine (HWND, HWND, LPCWSTR);
void Process_OpenDirectory (HWND, HWND, LPCWSTR);