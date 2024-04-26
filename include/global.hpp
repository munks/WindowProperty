/*
Window Property

Global Variable Naming Rule:
main.cpp - m_#Var
hook.cpp - h_#Var
menu.cpp - me_#Var
control.cpp - c_#Var
util.cpp - u_#Var
log.cpp - l_#Var
process.cpp - p_#Var
*/

#pragma once

//Definition
#define UNICODE
#define WINVER 0x0606
#undef __STRICT_ANSI__
#define WM_ICONNOTIFY 0x8001
#define _RESTRICT_WM

//Include Standard
#include <stdio.h>
#include <wchar.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <commctrl.h>
#include <TlHelp32.h>

//Include Custom
#include "main.hpp"
#include "control.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "list.hpp"
#include "util.hpp"
#include "process.hpp"
#include "dlgfunction.hpp"
#include "hook.hpp"
#include "vk.hpp"
#include "resource.h"

#ifdef _DEBUG
#include "debug.hpp" //Console Print Utilities
#endif

//Macro
#define IsTopMost(h) ((GetWindowLong(h, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0)
#define HasCaption(h) ((GetWindowLong(h, GWL_STYLE) & WS_CAPTION) != 0)

#define _repeat(v, i) for (unsigned int v = 0; v < i; v++)

//Process Loop Macro
/* Usage:

	ProcessLoop {
		#Actions
		#ProcessLoopName(); - wchar_t[MAX_PATH]
		#ProcessLoopID(); - ULONG
	} ProcessLoopEnd();
*/
#define ProcessLoop { \
							HANDLE lt_hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); \
							PROCESSENTRY32W lt_pe32 = {0}; \
							lt_pe32.dwSize = sizeof(PROCESSENTRY32W); \
							if(Process32FirstW(lt_hProcess, &lt_pe32)) { \
								while(Process32NextW(lt_hProcess, &lt_pe32))
#define ProcessLoopEnd() }	CloseHandle(lt_hProcess); }
#define ProcessLoopName() 	lt_pe32.szExeFile
#define ProcessLoopID() 	lt_pe32.th32ProcessID

//Window Loop Macro
/* Usage:

	WindowLoop(@PID) {
		#Actions
		#WindowLoopHandle(); - HWND
	} WindowLoopEnd();
*/
#define WindowLoop(pid) { \
							HWND lt_tempHwnd = FindWindow(NULL,NULL); \
							while (lt_tempHwnd != NULL) { \
								if (pid == Util_GetProcessID(lt_tempHwnd)) { \
									if (GetParent(lt_tempHwnd) == NULL)
#define WindowLoopEnd() } 	lt_tempHwnd = GetWindow(lt_tempHwnd, GW_HWNDNEXT); }}
#define WindowLoopHandle() 	lt_tempHwnd