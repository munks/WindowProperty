#include "global.hpp"

//Variable

wchar_t p_caption[MAX_PATH];
LONG_PTR p_currentProp[2];
TDATA p_tdata;
wchar_t p_exeFile[30];
DWORD p_dlgType;

//Internal

#define SetWindowRenew(h) SetWindowPos(h, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)

LRESULT CALLBACK HotkeyProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			SendDlgItemMessage(hwnd, ID_HOTKEY_MA, HKM_SETHOTKEY, Util_GetHotkey(HOTKEY_MOVE, HK_TYPE_ALL), 0);
			SendDlgItemMessage(hwnd, ID_HOTKEY_CC, HKM_SETHOTKEY, Util_GetHotkey(HOTKEY_CURSOR, HK_TYPE_ALL), 0);
			Hook_ClipHotkeyRegister(false);
			Hook_MoveHotkeyRegister(false);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_BUTTON_HK_CONFIRM) {
					if (EventMessage() == BN_CLICKED) {
						Util_SetHotkey(HOTKEY_MOVE, SendDlgItemMessage(hwnd, ID_HOTKEY_MA, HKM_GETHOTKEY, 0, 0));
						Util_SetHotkey(HOTKEY_CURSOR, SendDlgItemMessage(hwnd, ID_HOTKEY_CC, HKM_GETHOTKEY, 0, 0));
						EndDialog(hwnd, 0);
					}
					break;
				}
				DialogEvent(ID_BUTTON_HK_CANCEL) {
					if (EventMessage() == BN_CLICKED) {
						EndDialog(hwnd, 1);
					}
					break;
				}
				DialogEvent(ID_HOTKEY_MA)
				DialogEvent(ID_HOTKEY_CC) {
					if (DlgFunction_HotkeyValidCheck(hwnd, ID_HOTKEY_MA, ID_HOTKEY_CC)) {
						EnableWindow(GetDlgItem(hwnd, ID_BUTTON_HK_CONFIRM), TRUE);
					} else {
						EnableWindow(GetDlgItem(hwnd, ID_BUTTON_HK_CONFIRM), FALSE);
					}
					break;
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_CLOSE) {
			EndDialog(hwnd, 1);
			return 0;
		}
	}
	
	return 0;
}

LRESULT CALLBACK PropProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
	wchar_t cls[256];
	wchar_t val[256];
	DWORD len;
	HKEY key;
	HANDLE handle;
	FILETIME ft[4];
	
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			p_dlgType = (DWORD)lParam;
			
			//Set Style Button
			switch (p_dlgType) {
				case TYPE_DLG_PROP: {
					DlgFunction_PropInit(hwnd, &p_currentProp[0], &p_currentProp[1], false, NULL, true);
					
					handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Util_GetProcessID(p_tdata.hwnd));
					if (handle) {
						GetProcessTimes(handle, &p_tdata.ft, &ft[0], &ft[1], &ft[2]);
						CloseHandle(handle);
						p_tdata.timer = GetDlgItem(hwnd, ID_STATIC_TIME);
						CreateThread(NULL, 0, DlgFunction_SystemTimeLoop, &p_tdata, 0, NULL);
					} else {
						SetWindowText(GetDlgItem(hwnd, ID_STATIC_TIME), DLG_PROP_TIME_ACC_DENIED);
					}
					break;
				}
				case TYPE_DLG_FILTER: {
					DlgFunction_PropInit(hwnd, u_filter[0], u_filter[1], true, DLG_PROP_ADD_FILTER, false);
					break;
				}
			}
			
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_LBUTTONDOWN) {
			SetFocus(hwnd);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_BUTTON_PROP_ADD) {
					if (EventMessage() == BN_CLICKED) {
						switch (p_dlgType) {
							case TYPE_DLG_FILTER: {
								//Reset
								DlgFunction_ResetFilter(hwnd);
								break;
							}
						}
					}
					break;
				}
				//Confirm/Cancel
				DialogEvent(ID_BUTTON_PROP_CONFIRM) {
					if (EventMessage() == BN_CLICKED) {
						switch (p_dlgType) {
							case TYPE_DLG_PROP: {
								DlgFunction_SetProperties(hwnd, &p_currentProp[0], &p_currentProp[1]);
								break;
							}
							case TYPE_DLG_FILTER: {
								DlgFunction_SetFilter(hwnd);
								break;
							}
						}
						
						EndDialog(hwnd, 0);
					}
					break;
				}
				DialogEvent(ID_BUTTON_PROP_CANCEL) {
					if (EventMessage() == BN_CLICKED) {
						EndDialog(hwnd, 1);
					}
					break;
				}
				//Button On/Off (Multiple Style)
				DialogEvent(ID_BUTTON_STL_PW)
				DialogEvent(ID_BUTTON_STL_CAPTION)
				DialogEvent(ID_BUTTON_STL_OLW)
				DialogEvent(ID_BUTTON_EXSTL_PW)
				DialogEvent(ID_BUTTON_EXSTL_OLW)
				DialogEvent(ID_BUTTON_STL_TM)
				DialogEvent(ID_BUTTON_STL_GM)
				DialogEvent(ID_BUTTON_STL_TS)
				DialogEvent(ID_BUTTON_STL_SYSMENU)
				DialogEvent(ID_BUTTON_STL_DLGFRAME)
				DialogEvent(ID_BUTTON_STL_BORDER)
				DialogEvent(ID_BUTTON_STL_POPUP)
				DialogEvent(ID_BUTTON_EXSTL_TM)
				DialogEvent(ID_BUTTON_EXSTL_TW)
				DialogEvent(ID_BUTTON_EXSTL_WE)
				DialogEvent(ID_BUTTON_EXSTL_CE) {
					if (EventMessage() == BN_CLICKED) {
						DlgFunction_PropButtonState(hwnd, EventDialog(), (HWND)lParam);
					}
					break;
				}
				//Open Link (Style Description
				DialogEvent(ID_STATIC_STYLE) {
					if (EventMessage() == STN_CLICKED) {
						ShellExecute(NULL, L"open", LINK_STYLE, NULL, NULL, SW_SHOWNORMAL);
					}
					break;
				}
				DialogEvent(ID_STATIC_EXSTYLE) {
					if (EventMessage() == STN_CLICKED) {
						ShellExecute(NULL, L"open", LINK_EXSTYLE, NULL, NULL, SW_SHOWNORMAL);
					}
					break;
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_CLOSE) {
			EndDialog(hwnd, 1);
			return 0;
		}
	}
	
	return 0;
}

LRESULT CALLBACK NameProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
		
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			Edit_SetText(GetDlgItem(hwnd, ID_EDIT_INPUT), p_caption);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_BUTTON_NAME_INPUT) {
					if (EventMessage() == BN_CLICKED) {
						Edit_GetText(GetDlgItem(hwnd, ID_EDIT_INPUT), p_caption, MAX_PATH);
						EndDialog(hwnd, 0);
					}
					break;
				}
				DialogEvent(ID_BUTTON_NAME_CANCEL) {
					if (EventMessage() == BN_CLICKED) {
						EndDialog(hwnd, 1);
					}
					break;
				}
				DialogEvent(ID_EDIT_INPUT) {
					if (EventMessage() == EN_UPDATE) {
						tmphwnd = (HWND)lParam;
						
						if (Edit_GetTextLength(tmphwnd) > MAX_PATH) {
							Edit_GetText(tmphwnd, p_caption, MAX_PATH);
							Edit_SetText(tmphwnd, p_caption);
						}
					}
					break;
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	
	return 0;
}

int CheckAbsolutePath (const wchar_t* name) {
	wchar_t checker[MAX_PATH];
	
	GetModuleFileName(NULL, checker, MAX_PATH);
	wcscpy(wcsrchr(checker, L'\\') + 1, name);
	
	if (_waccess(checker, 0)) {
		Menu_InfoNotifyIcon(LOG_NO_PROGRAM, name, 3000);
		return 1;
	}
	
	return 0;
}

HINSTANCE ExecuteFromAbsolutePath (HWND main, LPCWSTR exe, LPCWSTR dll, ULONG pid) {
	wchar_t mainpath[MAX_PATH];
	wchar_t exepath[MAX_PATH];
	wchar_t params[MAX_PATH];
	
	GetModuleFileName(NULL, mainpath, MAX_PATH);
	*(wcsrchr(mainpath, L'\\') + 1) = L'\0';
	
	swprintf(exepath, L"%ls%ls", mainpath, exe);
	swprintf(params, L"%d \"%ls%ls\"", pid, mainpath, dll);
	
	#ifdef _DEBUG
	wprintf(L"ShellExecute: %ls %ls\n", exepath, params);
	#endif
	
	return ShellExecute(main, L"open", exepath, params, NULL, 0);
}

#define IntToStr(s, i) case i: wcscpy(s, L"##i##"); break;

wchar_t* FormatSEError (INT_PTR err) {
	static wchar_t str[25];
	
	switch (err) {
		IntToStr(str, ERROR_FILE_NOT_FOUND);
		IntToStr(str, ERROR_PATH_NOT_FOUND);
		IntToStr(str, ERROR_BAD_FORMAT);
		IntToStr(str, SE_ERR_ACCESSDENIED);
		IntToStr(str, SE_ERR_ASSOCINCOMPLETE);
		IntToStr(str, SE_ERR_DDEBUSY);
		IntToStr(str, SE_ERR_DDEFAIL);
		IntToStr(str, SE_ERR_DDETIMEOUT);
		IntToStr(str, SE_ERR_DLLNOTFOUND);
		IntToStr(str, SE_ERR_NOASSOC);
		IntToStr(str, SE_ERR_OOM);
		IntToStr(str, SE_ERR_SHARE);
		default:
			swprintf(str, L"%d", err);
			break;
	}
	
	return str;
}

//External

void Process_WindowPropChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	p_currentProp[0] = GetWindowLongPtr(hwnd, GWL_STYLE);
	p_currentProp[1] = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	p_tdata.hwnd = hwnd;
	ListView_GetItemText(GetDlgItem(m_main, ID_LIST), c_listViewIndex, 0, p_exeFile, 30);
	
	if (!DialogBoxParam(m_hInstance, MAKEINTRESOURCE(ID_DLG_PROP), m_main, PropProc, TYPE_DLG_PROP)) {
		if (IsWindow(hwnd)) {
			//Set Window Properties
			SetWindowLongPtr(hwnd, GWL_STYLE, p_currentProp[0]);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, p_currentProp[1]);
			//Set TOPMOST
			SetWindowPos(hwnd, p_currentProp[1] & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST,
						0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			
			Log_Message(LOG_FORMAT_NORMAL, LOG_SET_PROP, name, NULL);
		}
	}
	
	p_tdata.hwnd = NULL;
	p_tdata.timer = NULL;
	p_tdata.ft.dwHighDateTime = 0;
	p_tdata.ft.dwLowDateTime = 0;
}

void Process_WindowCaptionChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	GetWindowText(hwnd, p_caption, MAX_PATH);
	
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_NAME), m_main, NameProc)) {
		SetWindowText(hwnd, p_caption);
		
		Log_Message(LOG_FORMAT_NORMAL, LOG_CHANGE_CAPTION, name, NULL);
	}
}

void Process_WindowOpacityChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	int percent;
	BYTE alpha;
	LONG exstyle;
	
	percent = GetDlgItemInt(m_main, ID_EDIT_ALPHA, NULL, false);
	alpha = (BYTE)(((double)percent) / 100.0 * 255.0);
	exstyle = GetWindowExStyle(hwnd);
	
	SetWindowLong(hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
	SetWindowRenew(hwnd);
	
	Log_Message(LOG_FORMAT_OPACITY, LOG_SET_OPACITY, name, (wchar_t*)percent);
}

void Process_WindowFullScreenChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	LONG style;
	int screenX, screenY;
	bool isMaximized;
	
	screenX = GetSystemMetrics(SM_CXSCREEN);
	screenY = GetSystemMetrics(SM_CYSCREEN);
	isMaximized = IsMaximized(hwnd);
	
	if (isMaximized) {
		style = GetWindowStyle(hwnd) | WS_OVERLAPPEDWINDOW;
	} else {
		style = (GetWindowStyle(hwnd) & ~(WS_OVERLAPPEDWINDOW | WS_DLGFRAME)) | WS_POPUP;
	}
	
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	SetWindowLong(hwnd, GWL_STYLE, style);
	ShowWindow(hwnd, isMaximized ? SW_RESTORE : SW_MAXIMIZE);
	
	if (!isMaximized) {
		SetWindowPos(hwnd, HWND_TOP, 0, 0, screenX, screenY, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	} else {
		SetWindowRenew(hwnd);
	}
	
	Log_Message(LOG_FORMAT_EXTRA, LOG_CHANGE_SCREEN, name, isMaximized ? LOG_CHANGE_SCREEN_WINDOW : LOG_CHANGE_SCREEN_FULL);
}

void Process_WindowsDLLHook (HWND hwnd, HWND ctrl, LPCWSTR name) {
	HANDLE handle;
	BOOL iswow64;
	ULONG pid;
	wchar_t param[30];
	wchar_t exe[22];
	wchar_t dll[18];
	BYTE val;
	DWORD wda;
	INT_PTR err;
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
	if (!handle) {
		Util_PrintWindowsLastError();
		return;
	}
	
	IsWow64Process(handle, &iswow64);
	CloseHandle(handle);
	
	wcscpy(exe, !iswow64 ? L"dllinjector_x64.exe" : L"dllinjector_x86.exe");
	wcscpy(dll, !iswow64 ? L"cmdcapture_x64.dll" : L"cmdcapture_x86.dll");
	
	if (CheckAbsolutePath(exe)) { return; }
	if (CheckAbsolutePath(dll)) { return; }
	
	switch (GetDlgCtrlID(ctrl)) {
		case ID_BUTTON_CMD:
			val = 0x0;
			break;
		case ID_BUTTON_CAPTURE:
			val = 0x1;
			GetWindowDisplayAffinity(hwnd, &wda);
			if (!wda) {
				val |= 0x2;
			}
			RegSetValueEx(m_regkey, L"LastHookHWND", 0, REG_QWORD, (BYTE*)&hwnd, sizeof(HWND));
			break;
	}
	RegSetValueEx(m_regkey, L"LastHook", 0, REG_BINARY, &val, sizeof(BYTE));
	err = (INT_PTR)ExecuteFromAbsolutePath(m_main, exe, dll, pid);
	if (err > 32) {
		switch (GetDlgCtrlID(ctrl)) {
			case ID_BUTTON_CMD: {
				Log_Message(LOG_FORMAT_NORMAL, LOG_GET_COMMAND, name, NULL);
				break;
			}
			case ID_BUTTON_CAPTURE: {
				Log_Message(LOG_FORMAT_EXTRA, LOG_CHANGE_CAPTURE, name, wda ? L"NONE" : L"EXCLUDEFROMCAPTURE");
				break;
			}
		}
	} else {
		Menu_InfoNotifyIcon(LOG_SE_FAILED, FormatSEError(err), 3000);
	}
}

void Process_OpenDirectory (HWND hwnd, HWND ctrl, LPCWSTR name) {
	HANDLE handle;
	ULONG pid;
	wchar_t path[260];
	DWORD cnt = 260;
	INT_PTR err;
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
	if (!handle) {
		Util_PrintWindowsLastError();
		return;
	}
	
	QueryFullProcessImageName(handle, 0, path, &cnt);
	*wcsrchr(path, '\\') = '\0';
	CloseHandle(handle);
	
	
	
	err = (INT_PTR)ShellExecute(m_main, L"open", path, NULL, NULL, SW_SHOW);
	if (err > 32) {
		Log_Message(LOG_FORMAT_NORMAL, LOG_OPEN_DIRECTORY, name, NULL);
	} else {
		Menu_InfoNotifyIcon(LOG_SE_FAILED, FormatSEError(err), 3000);
	}
}

void Process_ChangeHotkey (HWND hwnd, HWND ctrl, LPCWSTR name) {
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_HOTKEY), m_main, HotkeyProc)) {
		Log_Message(LOG_FORMAT_HOTKEY, LOG_CHANGE_HOTKEY, NULL, NULL);
	}
}

void Process_ChangeFilter (HWND hwnd, HWND ctrl, LPCWSTR name) {
	if (!DialogBoxParam(m_hInstance, MAKEINTRESOURCE(ID_DLG_PROP), m_main, PropProc, TYPE_DLG_FILTER)) {
		Log_Message(LOG_FORMAT_FILTER, LOG_CHANGE_FILTER, NULL, NULL);
	}
}