#include "global.hpp"

//Variable

HWND m_main;
HWND m_changeButton;
HWND m_captionButton;
HWND m_showButton;
HWND m_capButton;
HWND m_moveButton;
HWND m_allButton;

HINSTANCE m_hInstance;
HFONT m_font;
HKEY m_regkey;

//Function

void Main_Close () {
	DeleteObject(m_font);
	RegCloseKey(m_regkey);
	Icon_RemoveNotifyIcon();
	FreeLibrary(c_comctlModule);
	PostQuitMessage(0);
}

LRESULT CALLBACK WindowProcMain (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
	wchar_t pidhwnd[10];
	wchar_t text[4];
	wchar_t name[30];
	BYTE alpha;
	int value;
	BOOL changed;
	
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_CREATE) {
			break;
		}
		WindowEvent(WM_NOTIFY) {
			//Prevent List-View Header Control
			if (ListViewMessage() == HDN_BEGINTRACK) {
				return true;
			}
			if (ListViewMessage() == HDN_ITEMCHANGING) {
				return true;
			}
			if (EventDialog() == ID_LIST) {
				if (ListViewMessage() == LVN_ITEMCHANGED) {
					//Set Selected Specific Value
					c_listViewIndex = ListView_GetNextItem(ListViewDialog(), -1, LVNI_FOCUSED | LVNI_SELECTED);
					ListView_GetItemText(ListViewDialog(), c_listViewIndex, 3, pidhwnd, 10);
					
					tmphwnd = (HWND)_wtoi(pidhwnd);
					changed = GetLayeredWindowAttributes(tmphwnd, NULL, &alpha, NULL);
					
					Control_SetChangeText(tmphwnd, m_changeButton, m_captionButton, m_showButton, m_capButton);
					swprintf(text, L"%d", changed ? (int)ceil(alpha / 255.0 * 100.0) : 100);
					SetDlgItemText(hwnd, ID_EDIT_ALPHA, text);
				}
			}
			break;
		}
		WindowEvent(WM_SYSCOMMAND) {
			if (wParam != SC_RESTORE) {
				break;
			}
		}
		WindowEvent(WM_SETFOCUS)
		WindowEvent(WM_MOUSEACTIVATE) {
			Control_RefreshListView();
			break;
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_MAIN) {
					//Enter Key Event
					if (GetFocus() == GetDlgItem(hwnd, ID_EDIT_ALPHA)) {
						SendMessage(GetDlgItem(hwnd, ID_BUTTON_OPACITY), BM_CLICK, 0, 0);
						SetFocus(GetDlgItem(hwnd, ID_EDIT_ALPHA));
					}
					break;
				}
				DialogEvent(ID_BUTTON_SCREEN)
				DialogEvent(ID_BUTTON_NAME)
				DialogEvent(ID_BUTTON_CAPTION)
				DialogEvent(ID_BUTTON_PIP)
				DialogEvent(ID_BUTTON_CHANGE)
				DialogEvent(ID_BUTTON_CMD)
				DialogEvent(ID_BUTTON_OPEN)
				DialogEvent(ID_BUTTON_SHOW)
				DialogEvent(ID_BUTTON_CAPTURE)
				DialogEvent(ID_BUTTON_OPACITY) {
					if (EventMessage() == BN_CLICKED) {
						//Change Window Priority
						void (*executionFunc)(HWND, HWND, const wchar_t*);
						
						switch (EventDialog()) {
							case ID_BUTTON_CHANGE:
								executionFunc = Process_WindowTopmostStateChange; break;
							case ID_BUTTON_NAME:
								executionFunc = Process_WindowCaptionChange; break;
							case ID_BUTTON_OPACITY:
								executionFunc = Process_WindowOpacityChange; break;
							case ID_BUTTON_CAPTION:
								executionFunc = Process_WindowCaptionDetach; break;
							case ID_BUTTON_SCREEN:
								executionFunc = Process_WindowFullScreenChange; break;
							case ID_BUTTON_PIP:
								executionFunc = Process_WindowPIPChange; break;
							case ID_BUTTON_CMD:
							case ID_BUTTON_CAPTURE:
								executionFunc = Process_WindowsDLLHook; break;
							case ID_BUTTON_OPEN:
								executionFunc = Process_OpenDirectory; break;
							case ID_BUTTON_SHOW:
								executionFunc = Process_ShowHideWindow; break;
						}
						
						ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 0, name, 30);
						if (name[0] == L'*') { wcscpy(name, name + 1); }
						
						//Selected Process Execution
						ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 3, pidhwnd, 10);
						if (IsWindow((HWND)_wtoi(pidhwnd))) {
							executionFunc((HWND)_wtoi(pidhwnd), (HWND)lParam, name);
						} else {
							Log_Message(L"윈도우를 찾을 수 없습니다. (HWND: %u)", (HWND)_wtoi(pidhwnd), name);
						}
						Control_RefreshListView();
					}
					break;
				}
				//In hook.cpp
				DialogEvent(ID_BUTTON_MOVE) {
					if (EventMessage() == BN_CLICKED) {
						changed = Button_GetCheck((HWND)lParam);
						
						changed ? Hook_MoveCallbackAttach() : Hook_MoveCallbackDetach();
						Icon_SetIconState(TN_MENU_MOVE, changed);
						RegSetValueEx(m_regkey, L"MoveActive", 0, REG_BINARY, (BYTE*)&changed, sizeof(BYTE));
					}
					break;
				}
				DialogEvent(ID_BUTTON_ALL) {
					if (EventMessage() == BN_CLICKED) {
						Control_RefreshListView();
					}
					break;
				}
				DialogEvent(ID_EDIT_ALPHA) {
					//Edit Control Limit(0~100)
					if (EventMessage() == EN_UPDATE) {
						value = GetDlgItemInt(hwnd, ID_EDIT_ALPHA, NULL, false);
						tmphwnd = (HWND)lParam;
						
						Edit_GetText(tmphwnd, text, 4);
						if (value < 0) { value = 0; }
						if (value > 100) { value = 100; }
						if (((text[0] != L'0') || (wcslen(text) < 2)) && (text[0] != L'\0')) {
							if (value == GetDlgItemInt(hwnd, ID_EDIT_ALPHA, NULL, false)) { break; }
						}
						
						swprintf(text, L"%d", value);
						Edit_SetText(tmphwnd, text);
						Edit_SetSel(tmphwnd, 3, 3);
					}
					break;
				}
				DialogEvent(ID_BUTTON_ICON) {
					Icon_ExecuteNotifyEvent(EventMessage());
					break;
				}
			}
			return 0;
		}
		//System Tray Notify Icon
		WindowEvent(WM_ICONNOTIFY) {
			if (lParam == WM_LBUTTONUP) {
				ShowWindow(m_main, SW_RESTORE);
			}
			if (lParam == WM_RBUTTONUP) {
				POINT cursor;
				
				GetCursorPos(&cursor);
				SetForegroundWindow(hwnd);
				TrackPopupMenu(Icon_GetMenu(), TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hwnd, NULL);
			}
			break;
		}
		//Prevent Sizing
		WindowEvent(WM_SIZING) {
			return true;
		}
		WindowEvent(WM_SIZE) {
			return true;
		}
		//Prevent Double Click Sizing
		WindowEvent(WM_NCLBUTTONDBLCLK) {
			return true;
		}
		//Hide Window Instead Of Closing
		WindowEvent(WM_CLOSE) {
			ShowWindow(hwnd, SW_HIDE);
			return 0;
		}
		//Not Used
		WindowEvent(WM_DESTROY) {
			Icon_RemoveNotifyIcon();
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	BYTE moveActive;
	DWORD size;
	WNDCLASSEX wc = {};
	
	//Duplicate Prevent
	if (FindWindow(WINDOW_MAIN_NAME, NULL) != NULL) {
		ShowWindow(FindWindow(WINDOW_MAIN_NAME, NULL), SW_RESTORE);
		SetForegroundWindow(FindWindow(WINDOW_MAIN_NAME, NULL));
		return 0;
	}
	
	//Set HINSTANCE
	m_hInstance = hInstance;
	
	//Open Registry
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Duality\\WindowProperty", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_regkey, NULL);
	
	//Control Init
	Control_InitDLL();
	
	//Create HFONT
	m_font = CreateFont(16,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,
						VARIABLE_PITCH | FF_ROMAN, L"Ebrima");
	
	//Create Window Class (Main)
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProcMain;
	wc.hInstance = hInstance;
	wc.lpszClassName = WINDOW_MAIN_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Util_CheckError((void*)(MAKELONG(RegisterClassEx(&wc), 0)));
	
	//Create Main Window
	m_main = CreateWindowEx(WS_EX_TOPMOST, WINDOW_MAIN_NAME, WINDOW_MAIN_CAPTION,
							WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
							CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
							NULL, NULL, hInstance, NULL);
	Util_CheckError(m_main);
	
	//Create Log Window
	Log_CreateWindow(m_main);

	//Create Button (CHANGE)
	m_changeButton = Control_CreateButton(m_main, BUTTON_CHANGE_CAPTION, BUTTON_CHANGE_TOOLTIP, false, 475, 10, 100, 30, ID_BUTTON_CHANGE);
	
	//Create Button (NAME)
	Control_CreateButton(m_main, BUTTON_NAME_CAPTION, BUTTON_NAME_TOOLTIP, false, 475, 50, 100, 30, ID_BUTTON_NAME);
	
	//Create Button (OPACITY)
	Control_CreateButton(m_main, BUTTON_OPACITY_CAPTION, BUTTON_OPACITY_TOOLTIP, false, 475, 90, 100, 30, ID_BUTTON_OPACITY);
	
	//Create Edit (OPACITY-ALPHA)
	Control_CreateEdit(m_main, BUTTON_OPACITY_TOOLTIP, 475, 125, 27, 20, ID_EDIT_ALPHA, L"100");
	
	//Create Static (OPACITY-PERCENTAGE)
	Control_CreateStatic(m_main, 510, 125, 16, 20, ID_STATIC_PERCENTAGE, L"%");
	
	//Create Button (FULL SCREEN)
	Control_CreateButton(m_main, BUTTON_SCREEN_CAPTION, BUTTON_SCREEN_TOOLTIP, false, 475, 150, 100, 30, ID_BUTTON_SCREEN);
	
	//Create Button (CAPTION)
	m_captionButton = Control_CreateButton(m_main, BUTTON_CAPTION_CAPTION, BUTTON_CAPTION_TOOLTIP, false, 475, 190, 100, 30, ID_BUTTON_CAPTION);
	
	//Create Button (Command Line)
	Control_CreateButton(m_main, BUTTON_CMD_CAPTION, BUTTON_CMD_TOOLTIP, false, 475, 230, 100, 30, ID_BUTTON_CMD);
	
	//Create Button (Open)
	Control_CreateButton(m_main, BUTTON_OPEN_CAPTION, BUTTON_OPEN_TOOLTIP, false, 475, 270, 100, 30, ID_BUTTON_OPEN);
	
	//Create Button (PIP Mode)
	//Control_CreateButton(m_main, BUTTON_PIP_CAPTION, BUTTON_PIP_TOOLTIP, false, 475, 310, 100, 30, ID_BUTTON_PIP);
	
	//Create Button (SHOW)
	m_showButton = Control_CreateButton(m_main, BUTTON_SHOW_CAPTION, BUTTON_SHOW_TOOLTIP, false, 475, 310, 100, 30, ID_BUTTON_SHOW);
	
	//Create Button (CAPTURE)
	m_capButton = Control_CreateButton(m_main, BUTTON_CAPTURE_CAPTION, BUTTON_CAPTURE_TOOLTIP, false, 475, 350, 100, 30, ID_BUTTON_CAPTURE);
	
	//Create Button (ALL)
	m_allButton = Control_CreateButton(m_main, BUTTON_ALL_CAPTION, BUTTON_ALL_TOOLTIP, true, 475, 390, 100, 30, ID_BUTTON_ALL);
	
	//Create Button (MOVE)
	m_moveButton = Control_CreateButton(m_main, BUTTON_MOVE_CAPTION, BUTTON_MOVE_TOOLTIP, true, 475, 430, 100, 30, ID_BUTTON_MOVE);
	
	//Create List-View
	Control_CreateListView(m_main, LIST_TOOLTIP, 10, 10, 450, 440, ID_LIST);
	Control_RefreshListView();
	
	//Add System Tray Notify Icon
	Icon_RemoveNotifyIcon();
	Icon_AddNotifyIcon(m_main);
	Icon_MakeMenu();
	
	//Get Registry (MOVE)
	moveActive = 0;
	size = sizeof(BYTE);
	RegGetValue(m_regkey, NULL, L"MoveActive", RRF_RT_REG_BINARY, NULL, &moveActive, &size);
	Button_SetCheck(m_moveButton, moveActive ? BST_CHECKED : BST_UNCHECKED);
	Icon_SetIconState(TN_MENU_MOVE, moveActive);
	if (moveActive) { Hook_MoveCallbackAttach(); }
	
	//Get Registry (INIT)
	Icon_SetIconState(TN_MENU_INIT, !RegGetValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"WindowProperty", RRF_RT_REG_SZ, NULL, NULL, NULL));
	
	//Show Window (Main)
	UpdateWindow(m_main);
	if (strcmp(pCmdLine, "-hide")) {
		ShowWindow(m_main, SW_SHOW);
	}
	
	//Message Loop
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if(!IsDialogMessage(m_main, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}