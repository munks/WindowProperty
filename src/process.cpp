#include "global.hpp"

//Variable

wchar_t p_caption[MAX_PATH];

//Internal

#define SetWindowRenew(h) SetWindowPos(h, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)

LRESULT CALLBACK InputProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
				DialogEvent(ID_BUTTON_INPUT) {
					if (EventMessage() == BN_CLICKED) {
						Edit_GetText(GetDlgItem(hwnd, ID_EDIT_INPUT), p_caption, MAX_PATH);
						EndDialog(hwnd, 0);
					}
					break;
				}
				DialogEvent(ID_BUTTON_CANCEL) {
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

void Process_WindowTopmostStateChange (HWND hwnd, HWND main, LPCWSTR name) {
	SetWindowPos(hwnd, IsTopMost(hwnd) ? HWND_NOTOPMOST : HWND_TOPMOST,
				0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	
	Log_Message(L"윈도우를 %s로 설정했습니다. (%s)", IsTopMost(hwnd) ? L"TopMost" : L"NoTopMost", name);
}

void Process_WindowCaptionChange (HWND hwnd, HWND main, LPCWSTR name) {
	GetWindowText(hwnd, p_caption, MAX_PATH);
	if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_INPUT), main, InputProc)) {
		SetWindowText(hwnd, p_caption);
		Log_Message(L"윈도우 제목을 변경했습니다. (%s)", name);
	}
}

void Process_WindowOpacityChange (HWND hwnd, HWND main, LPCWSTR name) {
	int percent;
	BYTE alpha;
	LONG exstyle;
	
	percent = GetDlgItemInt(main, ID_EDIT_ALPHA, NULL, false);
	alpha = (BYTE)(((double)percent) / 100.0 * 255.0);
	exstyle = GetWindowExStyle(hwnd);
	
	SetWindowLong(hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
	SetWindowRenew(hwnd);
	
	Log_Message(L"윈도우 불투명도를 %d%%로 설정했습니다. (%s)", percent, name);
}

void Process_WindowCaptionDetach (HWND hwnd, HWND main, LPCWSTR name) {
	LONG hwndStyle = GetWindowStyle(hwnd);
	
	SetWindowLong(hwnd, GWL_STYLE,
					(hwndStyle & WS_CAPTION) ? hwndStyle & ~WS_CAPTION : hwndStyle | WS_CAPTION);
	SetWindowRenew(hwnd);
	
	Log_Message(L"제목 표시줄을 %s했습니다. (%s)", (hwndStyle & WS_CAPTION) ? L"제거" : L"추가", name);
}

void Process_WindowFullScreenChange (HWND hwnd, HWND main, LPCWSTR name) {
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
	
	Log_Message(L"윈도우를 %s로 설정했습니다. (%s)", isMaximized ? L"창 모드" : L"전체 화면", name);
}

void Process_WindowPIPChange (HWND hwnd, HWND main, LPCWSTR name) {
	LONG style;
	int screenX, screenY;
	
	style = GetWindowStyle(hwnd);
	screenX = GetSystemMetrics(SM_CXSCREEN);
	screenY = GetSystemMetrics(SM_CYSCREEN);
	
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	SetWindowLong(hwnd, GWL_STYLE, (style & ~(WS_OVERLAPPEDWINDOW | WS_DLGFRAME)) | WS_POPUP);
	
	ShowWindow(hwnd, SW_RESTORE);
	SetWindowPos(hwnd, HWND_TOPMOST,
				screenX - (screenX / 20) - (screenX / 3),
				screenY - (screenY / 20) - (screenY / 3),
				screenX / 3, screenY / 3, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	
	Log_Message(L"윈도우를 PIP 모드로 설정했습니다. (%s)", name);
}

void Process_CommandLine (HWND hwnd, HWND main, LPCWSTR name) {
	HANDLE handle;
	BOOL iswow64;
	ULONG pid;
	wchar_t param[30];
	wchar_t exe[22];
	wchar_t dll[18];
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	IsWow64Process(handle, &iswow64);
	CloseHandle(handle);
	
	wcscpy(exe, !iswow64 ? L"dllinjector_x64.exe" : L"dllinjector_x86.exe");
	wcscpy(dll, !iswow64 ? L"cmdline_x64.dll" : L"cmdline_x86.dll");
	
	if (CheckAbsolutePath(exe)) { return; }
	if (CheckAbsolutePath(dll)) { return; }
	
	ExecuteFromAbsolutePath(main, exe, dll, pid);
	
	Log_Message(L"프로세스의 명령줄 인수를 받아왔습니다. (%s)", name);
}

void Process_OpenDirectory (HWND hwnd, HWND main, LPCWSTR name) {
	HANDLE handle;
	ULONG pid;
	wchar_t path[260];
	DWORD cnt = 260;
	
	pid = Util_GetProcessID(hwnd);
	
	handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	QueryFullProcessImageName(handle, 0, path, &cnt);
	*wcsrchr(path, '\\') = '\0';
	CloseHandle(handle);
	
	ShellExecute(main, L"open", path, NULL, NULL, SW_SHOW);
	
	Log_Message(L"프로그램의 경로 폴더를 열었습니다. (%s)", name);
}