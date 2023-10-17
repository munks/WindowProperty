#include "global.hpp"

//Variable

wchar_t p_caption[MAX_PATH];
LONG_PTR p_currentProp[2];

#define LINK_STYLE L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/window-styles"
#define LINK_EXSTYLE L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/extended-window-styles"

//Internal

#define SetWindowRenew(h) SetWindowPos(h, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)

bool DlgsIsChecked (HWND hwnd, int init, int id1, int id2, int id3, int id4, int id5) {
	if (id1 != -1) {
		if (Button_GetCheck(GetDlgItem(hwnd, init + id1)) != BST_CHECKED) {
			return false;
		}
	}
	if (id2 != -1) {
		if (Button_GetCheck(GetDlgItem(hwnd, init + id2)) != BST_CHECKED) {
			return false;
		}
	}
	if (id3 != -1) {
		if (Button_GetCheck(GetDlgItem(hwnd, init + id3)) != BST_CHECKED) {
			return false;
		}
	}
	if (id4 != -1) {
		if (Button_GetCheck(GetDlgItem(hwnd, init + id4)) != BST_CHECKED) {
			return false;
		}
	}
	if (id5 != -1) {
		if (Button_GetCheck(GetDlgItem(hwnd, init + id5)) != BST_CHECKED) {
			return false;
		}
	}
	return true;
}

void CheckMultiStyleButton (HWND hwnd, int id, HWND ctrl) {
	BOOL state = Button_GetCheck(ctrl);
	
	switch (id) {
		case ID_BUTTON_STL_PW:
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 19), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 23), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 31), state);
			break;
		case ID_BUTTON_STL_CAPTION:
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 22), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 23), state);
			break;
		case ID_BUTTON_STL_OLW:
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 16), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 17), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 18), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 19), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 22), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON + 23), state);
			break;
		case ID_BUTTON_EXSTL_PW:
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON_EX + 3), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON_EX + 7), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON_EX + 8), state);
			break;
		case ID_BUTTON_EXSTL_OLW:
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON_EX + 8), state);
			Button_SetCheck(GetDlgItem(hwnd, PROP_BUTTON_EX + 9), state);
			break;
	}
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), DlgsIsChecked(hwnd, PROP_BUTTON, 22, 23, -1, -1, -1) ? BST_CHECKED : BST_UNCHECKED);
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), DlgsIsChecked(hwnd, PROP_BUTTON, 16, 17, 18, 19, 34) ? BST_CHECKED : BST_UNCHECKED);
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_PW), DlgsIsChecked(hwnd, PROP_BUTTON, 19, 23, 31, -1, -1) ? BST_CHECKED : BST_UNCHECKED);
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), DlgsIsChecked(hwnd, PROP_BUTTON_EX, 3, 7, 8, -1, -1) ? BST_CHECKED : BST_UNCHECKED);
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), DlgsIsChecked(hwnd, PROP_BUTTON_EX, 8, 9, -1, -1, -1) ? BST_CHECKED : BST_UNCHECKED);
}
LRESULT CALLBACK PropProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
		
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			//Set Style Button
			for (int i = 0; i < 32; i++) {
				if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
					Button_SetCheck(tmphwnd, p_currentProp[0] & (1 << i) ? BST_CHECKED : BST_UNCHECKED);
				}
				if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
					Button_SetCheck(tmphwnd, p_currentProp[1] & (1 << i) ? BST_CHECKED : BST_UNCHECKED);
				}
			}
			//POPUPWINDOW
			if (DlgsIsChecked(hwnd, PROP_BUTTON, 19, 23, 31, -1, -1)) {
				Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_PW), BST_CHECKED);
			}
			//CAPTION
			if (DlgsIsChecked(hwnd, PROP_BUTTON, 22, 23, -1, -1, -1)) {
				Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), BST_CHECKED);
			}
			//OVERLAPPEDWINDOW
			if (DlgsIsChecked(hwnd, PROP_BUTTON, 16, 17, 18, 19, 34)) {
				Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), BST_CHECKED);
			}
			//PALETTEWINDOW (Extended)
			if (DlgsIsChecked(hwnd, PROP_BUTTON_EX, 3, 7, 8, -1, -1)) {
				Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), BST_CHECKED);
			}
			//OVERLAPPEDWINDOW (Extended)
			if (DlgsIsChecked(hwnd, PROP_BUTTON_EX, 8, 9, -1, -1, -1)) {
				Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), BST_CHECKED);
			}
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
						CheckMultiStyleButton(hwnd, EventDialog(), (HWND)lParam);
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
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
		Log_Message(L"명령줄 인수를 받아오는 프로그램이 없습니다. (%ls)", name);
		return 1;
	}
	
	return 0;
}

void ExecuteFromAbsolutePath (HWND main, LPCWSTR exe, LPCWSTR dll, ULONG pid) {
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
	
	ShellExecute(main, L"open", exepath, params, NULL, 0);
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
		
		Log_Message(L"윈도우 설정을 변경했습니다. (%ls)", name);
	}
}

void Process_WindowCaptionChange (HWND hwnd, HWND ctrl, LPCWSTR name) {
	GetWindowText(hwnd, p_caption, MAX_PATH);
	
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_NAME), GetAncestor(ctrl, GA_PARENT), NameProc)) {
		SetWindowText(hwnd, p_caption);
		
		Log_Message(L"윈도우 제목을 변경했습니다. (%ls)", name);
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
	
	Log_Message(L"윈도우 불투명도를 %d%%로 설정했습니다. (%ls)", percent, name);
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
	
	Log_Message(L"윈도우를 %ls로 설정했습니다. (%ls)", isMaximized ? L"창 모드" : L"전체 화면", name);
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
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	IsWow64Process(handle, &iswow64);
	CloseHandle(handle);
	
	wcscpy(exe, !iswow64 ? L"dllinjector_x64.exe" : L"dllinjector_x86.exe");
	wcscpy(dll, !iswow64 ? L"cmdline_x64.dll" : L"cmdline_x86.dll");
	
	if (CheckAbsolutePath(exe)) { return; }
	if (CheckAbsolutePath(dll)) { return; }
	
	switch (GetDlgCtrlID(ctrl)) {
		case ID_BUTTON_CMD:
			val = 0x0;
			Log_Message(L"프로세스의 명령줄 인수를 받아왔습니다. (%ls)", name);
			break;
		case ID_BUTTON_CAPTURE:
			val = 0x1;
			GetWindowDisplayAffinity(hwnd, &wda);
			if (!wda) {
				val |= 0x2;
			}
			RegSetValueEx(m_regkey, L"LastHookHWND", 0, REG_QWORD, (BYTE*)&hwnd, sizeof(HWND));
			Log_Message(L"윈도우의 캡쳐 가능 여부를 %ls 상태로 변경했습니다. (%ls)", wda ? L"NONE" : L"EXCLUDEFROMCAPTURE", name);
			break;
	}
	RegSetValueEx(m_regkey, L"LastHook", 0, REG_BINARY, &val, sizeof(BYTE));
	ExecuteFromAbsolutePath(GetAncestor(ctrl, GA_PARENT), exe, dll, pid);
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
	
	Log_Message(L"프로그램의 경로 폴더를 열었습니다. (%ls)", name);
}