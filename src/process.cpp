#include "global.hpp"

//Variable

wchar_t p_caption[MAX_PATH];
LONG_PTR p_currentProp[2];

//Internal

#define SetWindowRenew(h) SetWindowPos(h, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)

LRESULT CALLBACK PropProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
		
	#ifdef _DEBUG
	printf("Dialog");
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			//Set Style Button
			Control_PropDialogInit(hwnd, &p_currentProp[0], &p_currentProp[1], false);
			
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_LBUTTONDOWN) {
			SetFocus(hwnd);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				//Confirm/Cancel
				DialogEvent(ID_BUTTON_PROP_CONFIRM) {
					if (EventMessage() == BN_CLICKED) {
						p_currentProp[0] = 0;
						p_currentProp[1] = 0;
						
						for (int i = 0; i < 32; i++) {
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
								if (Button_GetCheck(tmphwnd) == BST_CHECKED) {
									p_currentProp[0] |= 1 << i;
								}
							}
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
								if (Button_GetCheck(tmphwnd) == BST_CHECKED) {
									p_currentProp[1] |= 1 << i;
								}
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
						Control_PropDialogButtonState(hwnd, EventDialog(), (HWND)lParam);
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
		Log_Message(LOG_NO_PROGRAM, name);
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
	
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_PROP), GetAncestor(ctrl, GA_PARENT), PropProc)) {
		//Set Window Properties
		SetWindowLongPtr(hwnd, GWL_STYLE, p_currentProp[0]);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, p_currentProp[1]);
		//Set TOPMOST (It does not apply with SetWindowLongPtr)
		SetWindowPos(hwnd, p_currentProp[1] & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST,
					0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		
		Log_Message(LOG_SET_PROP, name);
	}
}

void Process_WindowCaptionChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	GetWindowText(hwnd, p_caption, MAX_PATH);
	
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_NAME), GetAncestor(ctrl, GA_PARENT), NameProc)) {
		SetWindowText(hwnd, p_caption);
		
		Log_Message(LOG_CHANGE_CAPTION, name);
	}
}

void Process_WindowOpacityChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	int percent;
	BYTE alpha;
	LONG exstyle;
	
	percent = GetDlgItemInt(GetAncestor(ctrl, GA_PARENT), ID_EDIT_ALPHA, NULL, false);
	alpha = (BYTE)(((double)percent) / 100.0 * 255.0);
	exstyle = GetWindowExStyle(hwnd);
	
	SetWindowLong(hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
	SetWindowRenew(hwnd);
	
	Log_Message(LOG_SET_OPACITY, percent, name);
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
	
	Log_Message(LOG_CHANGE_SCREEN, isMaximized ? LOG_CHANGE_SCREEN_WINDOW : LOG_CHANGE_SCREEN_FULL, name);
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
	
	handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
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
	err = (INT_PTR)ExecuteFromAbsolutePath(GetAncestor(ctrl, GA_PARENT), exe, dll, pid);
	if (err > 32) {
		switch (GetDlgCtrlID(ctrl)) {
			case ID_BUTTON_CMD: {
				Log_Message(LOG_GET_COMMAND, name);
				break;
			}
			case ID_BUTTON_CAPTURE: {
				Log_Message(LOG_CHANGE_CAPTURE, wda ? L"NONE" : L"EXCLUDEFROMCAPTURE", name);
				break;
			}
		}
	} else {
		Log_Message(LOG_SE_FAILED, FormatSEError(err));
	}
}

void Process_OpenDirectory (HWND hwnd, HWND ctrl, LPCWSTR name) {
	HANDLE handle;
	ULONG pid;
	wchar_t path[260];
	DWORD cnt = 260;
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	QueryFullProcessImageName(handle, 0, path, &cnt);
	*wcsrchr(path, '\\') = '\0';
	CloseHandle(handle);
	
	ShellExecute(GetAncestor(ctrl, GA_PARENT), L"open", path, NULL, NULL, SW_SHOW);
	
	Log_Message(LOG_OPEN_DIRECTORY, name);
}