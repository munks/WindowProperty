#include "global.hpp"

//Variable

HWND m_main;
HBRUSH m_hbrush;
HINSTANCE m_hInstance;
HFONT m_font;
HKEY m_regkey;
HKEY m_regset;
HKEY m_regrec;

//External

void Main_Close () {
	DestroyMenu(me_menu);
	DeleteObject(m_font);
	RegCloseKey(m_regkey);
	RegCloseKey(m_regset);
	RegCloseKey(m_regrec);
	Menu_RemoveNotifyIcon();
	Thread_Close();
	FreeLibrary(c_comctlModule);
	PostQuitMessage(0);
}

//Internal
#define CreateButtonMacro(h, id, cb, x, y, cx, cy) Control_CreateButton(h, BUTTON_##id##_CAPTION, BUTTON_##id##_TOOLTIP, cb, x, y, cx, cy, ID_BUTTON_##id)

void Main_DeleteRegistryVer0() {
	int idx = 0;
	DWORD len;
	wchar_t key[256];
	
	while (RegEnumKeyEx(m_regset, idx, key, &(len = 256), NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
		RegDeleteTree(m_regset, key);
		RegDeleteKey(m_regset, key);
	}
}

void Main_VersionCheck (DWORD ver) {
	DWORD reg, size;
	
	RegGetValue(m_regkey, NULL, L"Version", RRF_RT_REG_DWORD, NULL, &(reg = 0), &(size = sizeof(DWORD)));
	
	switch (reg) {
		case 0: Main_DeleteRegistryVer0();
		case 1:
		case 2:
		case 3: break;
	}
	
	RegSetValueEx(m_regkey, L"Version", 0, REG_DWORD, (BYTE*)&ver, sizeof(DWORD));
}

LRESULT CALLBACK MainProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
	wchar_t pidhwnd[10];
	wchar_t text[4];
	wchar_t name[30];
	wchar_t path[MAX_PATH];
	BYTE alpha;
	int value;
	BOOL changed;
	POINT cursor;
	RECT rect;
	
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_NOTIFY) {
			NotifyEventCase(ListViewMessage()) {
				NotifyEvent(HDN_BEGINTRACK)
				NotifyEvent(HDN_ITEMCHANGING) {
					//Prevent List-View Header Control
					return true;
				}
				NotifyEvent(LVN_ITEMCHANGED) {
					//Set Selected Specific Value
					c_listViewIndex = ListView_GetNextItem(ListViewDialog(), -1, LVNI_FOCUSED | LVNI_SELECTED);
					ListView_GetItemText(ListViewDialog(), c_listViewIndex, 3, pidhwnd, 10);
					
					tmphwnd = (HWND)_wtoi(pidhwnd);
					changed = GetLayeredWindowAttributes(tmphwnd, NULL, &alpha, NULL);
					
					Button_SetText(GetDlgItem(hwnd, ID_BUTTON_CAPTURE),
								Util_GetWDAState(tmphwnd) ? BUTTON_CAPTURE_CAPTION : BUTTON_CAPTURE_CAPTION_2);
					swprintf(text, L"%d", changed ? (int)ceil(alpha / 255.0 * 100.0) : 100);
					SetDlgItemText(hwnd, ID_EDIT_ALPHA, text);
					break;
				}
			}
			break;
		}
		WindowEvent(WM_COPYDATA) {
			if (((COPYDATASTRUCT*)lParam)->dwData == 1) {
				wcscpy(path, (LPWSTR)((COPYDATASTRUCT*)lParam)->lpData);
				Thread_CreateThreadProcess(path);
			}
			break;
		}
		WindowEvent(WM_SYSCOMMAND) {
			if (wParam != SC_RESTORE) {
				break;
			}
		}
		WindowEvent(WM_SHOWWINDOW)
		WindowEvent(WM_SETFOCUS) {
			Control_RefreshListView();
			break;
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				DialogEvent(ID_BUTTON_PROP)
				DialogEvent(ID_BUTTON_NAME)
				DialogEvent(ID_BUTTON_OPACITY)
				DialogEvent(ID_BUTTON_SCREEN)
				DialogEvent(ID_BUTTON_CMD)
				DialogEvent(ID_BUTTON_CAPTURE)
				DialogEvent(ID_BUTTON_OPEN)
				DialogEvent(ID_BUTTON_MODULE)
				DialogEvent(ID_BUTTON_RUNTIME)
				DialogEvent(ID_BUTTON_HOTKEY)
				DialogEvent(ID_BUTTON_FILTER) {
					if (EventMessage() == BN_CLICKED) {
						//Change Window Property
						void (*executionFunc)(HWND, HWND, LPCWSTR) = NULL;
						bool absolute = false;
						
						switch (EventDialog()) {
							case ID_BUTTON_PROP:
								executionFunc = Process_WindowPropChange; break;
							case ID_BUTTON_NAME:
								executionFunc = Process_WindowCaptionChange; break;
							case ID_BUTTON_OPACITY:
								executionFunc = Process_WindowOpacityChange; break;
							case ID_BUTTON_SCREEN:
								executionFunc = Process_WindowFullScreenChange; break;
							case ID_BUTTON_CMD:
							case ID_BUTTON_CAPTURE:
								executionFunc = Process_WindowsDLLHook; break;
							case ID_BUTTON_OPEN:
								executionFunc = Process_OpenDirectory; break;
							case ID_BUTTON_MODULE:
								executionFunc = Process_EnumModule; break;
							case ID_BUTTON_RUNTIME:
								executionFunc = Process_RuntimeChecker; break;
							case ID_BUTTON_HOTKEY:
								executionFunc = Process_ChangeHotkey; absolute = true; break;
							case ID_BUTTON_FILTER:
								executionFunc = Process_ChangeFilter; absolute = true; break;
						}
						if (absolute) {
							executionFunc(NULL, NULL, NULL);
						} else {
							if (c_listViewIndex == -1) { break; }
							
							if (executionFunc == NULL) { break; }
							ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 0, name, 30);
							if (name[0] == L'*') { wcscpy(name, name + 1); }
							
							//Selected Process Execution
							ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 3, pidhwnd, 10);
							if (IsWindow((HWND)_wtoi(pidhwnd)) && !IsHungAppWindow((HWND)_wtoi(pidhwnd))) {
								executionFunc((HWND)_wtoi(pidhwnd), (HWND)lParam, name);
							} else {
								Menu_InfoNotifyIcon(name, LOG_NO_OR_NR_WINDOW, 3000);
							}
						}
						Control_RefreshListView();
					}
					break;
				}
				//In hook.cpp
				DialogEvent(ID_BUTTON_MOVE) {
					if (EventMessage() == BN_CLICKED) {
						Hook_MoveHotkeyRegister(Button_GetCheck((HWND)lParam));
					}
					break;
				}
				DialogEvent(ID_BUTTON_CLIP) {
					if (EventMessage() == BN_CLICKED) {
						Hook_ClipHotkeyRegister(Button_GetCheck((HWND)lParam));
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
					Menu_ExecuteNotifyEvent(EventMessage());
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
				GetCursorPos(&cursor);
				SetForegroundWindow(hwnd);
				TrackPopupMenu(me_menu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hwnd, NULL);
			}
			break;
		}
		//Prevent Sizing
		WindowEvent(WM_SIZING)
		WindowEvent(WM_SIZE)
		WindowEvent(WM_NCLBUTTONDBLCLK) {
			return true;
		}
		//Hide Window Instead Of Closing
		WindowEvent(WM_CLOSE) {
			AnimateWindow(hwnd, 200, AW_HIDE | AW_BLEND);
			return 0;
		}
		//Not Used
		WindowEvent(WM_DESTROY) {
			Menu_RemoveNotifyIcon();
			PostQuitMessage(0);
			return 0;
		}
		//Hotkey Processing
		WindowEvent(WM_HOTKEY) {
			if (wParam == HOTKEY_MOVE) {
				Hook_MouseHook();
				
				#ifdef _DEBUG
				puts("Hotkey Pressed: Move Active");
				#endif
				break;
			}
			if (wParam == HOTKEY_CURSOR) {
				Hook_ClipCursor();
				
				#ifdef _DEBUG
				puts("Hotkey Pressed: Clip Cursor");
				#endif
				break;
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
	bool wndHide = false;
	BYTE regval;
	DWORD size;
	WNDCLASSEX wc = {};
	HWND hwndFind;
	LPWSTR* cmdArgs;
	int cmdArgc;
	COPYDATASTRUCT cds = {};
	HKEY hkey;
	LSTATUS result;
	
	//Command Line Processing
	cmdArgs = CommandLineToArgvW(pCmdLine, &cmdArgc);
	if (cmdArgs != NULL) {
		for (int i = 0; i < cmdArgc; i++) {
			wprintf(L"Command Line: %ls\n", cmdArgs[i]);
			if (wcscmp(cmdArgs[i], L"-hide") == 0) { wndHide = true; }
			if (wcscmp(cmdArgs[i], L"-rtcheck") == 0) {
				i++;
				if (i < cmdArgc) {
					wprintf(L"Command Line: %ls\n", cmdArgs[i]);
					cds.dwData = 1;
					cds.cbData = wcslen(cmdArgs[i]) * sizeof(wchar_t);
					cds.lpData = cmdArgs[i];
				}
			}
		}
	}
	
	//Duplicate Prevent
	hwndFind = FindWindow(WINDOW_MAIN_NAME, NULL);
	if (hwndFind != NULL) {
		SendMessage(hwndFind, WM_COPYDATA, 0, (LPARAM)&cds);
		ShowWindow(hwndFind, SW_RESTORE);
		SetForegroundWindow(hwndFind);
		return 0;
	}
	
	#ifdef _DEBUG
	#if LANG == KOKR
	setlocale(LC_ALL, "ko-KR");
	#endif
	#endif
	
	//Open Registry
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Duality\\WindowProperty", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_regkey, NULL);
	RegCreateKeyEx(m_regkey, L"Settings", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_regset, NULL);
	RegCreateKeyEx(m_regkey, L"Records", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_regrec, NULL);
	
	Main_VersionCheck(1);
	
	//Set Global Variables
	m_hInstance = hInstance;
	m_hbrush = CreateSolidBrush(RGB(240,240,240));
	m_font = CreateFont(16,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,
						VARIABLE_PITCH | FF_ROMAN, L"Ebrima");
	RegGetValue(m_regset, NULL, L"FilterIncludeStyle", RRF_RT_DWORD, NULL, &u_filter[0][0], &(size = 4));
	RegGetValue(m_regset, NULL, L"FilterExcludeStyle", RRF_RT_DWORD, NULL, &u_filter[0][1], &(size = 4));
	RegGetValue(m_regset, NULL, L"FilterIncludeExStyle", RRF_RT_DWORD, NULL, &u_filter[1][0], &(size = 4));
	RegGetValue(m_regset, NULL, L"FilterExcludeExStyle", RRF_RT_DWORD, NULL, &u_filter[1][1], &(size = 4));
	
	//Control Init
	Control_InitDLL();
	
	//Thread Init
	Thread_Init();
	
	//Runtime Checker Start
	if (cds.dwData == 1) {
		Thread_CreateThreadProcess((LPCWSTR)cds.lpData);
	}
	
	//Create Window Class (Main)
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MainProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WINDOW_MAIN_NAME;
	wc.hbrBackground = m_hbrush;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Util_CheckError((void*)(MAKELONG(RegisterClassEx(&wc), 0)));
	
	//Create Main Window
	m_main = CreateWindowEx(WS_EX_TOPMOST, WINDOW_MAIN_NAME, WINDOW_MAIN_CAPTION,
							WS_OVERLAPPED | WS_CAPTION | WS_POPUPWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT, 600, 580,
							NULL, NULL, hInstance, NULL);
	Util_CheckError(m_main);
	
	//Create Log/Hotkey Window
	Log_CreateWindow(m_main);
	List_CreateWindow(m_main);
	
	//Check Hotkey
	if (Util_GetHotkey(HOTKEY_MOVE, HK_TYPE_VK) == 0) {
		Util_SetHotkey(HOTKEY_MOVE, VK_F3);
	}
	if (Util_GetHotkey(HOTKEY_CURSOR, HK_TYPE_VK) == 0) {
		Util_SetHotkey(HOTKEY_CURSOR, VK_F4);
	}
	
	//Create Button (PROP)
	CreateButtonMacro(m_main, PROP, false, 475, 10, 100, 30);
	
	//Create Button (NAME)
	CreateButtonMacro(m_main, NAME, false, 475, 50, 100, 30);
	
	//Create Button (OPACITY)
	CreateButtonMacro(m_main, OPACITY, false, 475, 90, 100, 30);
	
	//Create Edit (OPACITY-ALPHA)
	Control_CreateEdit(m_main, BUTTON_OPACITY_TOOLTIP, 475, 125, 27, 20, ID_EDIT_ALPHA, L"100");
	
	//Create Static (OPACITY-PERCENTAGE)
	Control_CreateStatic(m_main, 504, 126, 16, 20, ID_STATIC_PERCENTAGE, L"%");
	
	//Create Button (FULL SCREEN)
	CreateButtonMacro(m_main, SCREEN, false, 475, 150, 100, 30);
	
	//Create Button (Command Line)
	CreateButtonMacro(m_main, CMD, false, 475, 190, 100, 30);
	
	//Create Button (CAPTURE)
	CreateButtonMacro(m_main, CAPTURE, false, 475, 230, 100, 30);
	
	//Create Button (OPEN)
	CreateButtonMacro(m_main, OPEN, false, 475, 270, 100, 30);
	
	//Create Button (HOTKEY)
	CreateButtonMacro(m_main, HOTKEY, false, 475, 310, 100, 30);
	
	//Create Button (FILTER)
	CreateButtonMacro(m_main, FILTER, false, 475, 350, 100, 30);
	
	//Create Button (MODULE)
	CreateButtonMacro(m_main, MODULE, false, 475, 390, 100, 30);
	
	//Create Button (MODULE)
	CreateButtonMacro(m_main, RUNTIME, false, 475, 430, 100, 30);
	
	//Create Button (MOVE)
	CreateButtonMacro(m_main, MOVE, true, 475, 470, 100, 30);
	
	//Create Button (CLIP)
	CreateButtonMacro(m_main, CLIP, true, 475, 510, 100, 30);
	
	//Create List-View
	Control_CreateListView(m_main, LIST_TOOLTIP, 10, 10, 450, 520, ID_LIST);
	Control_RefreshListView();
	
	//Add System Tray Notify Icon
	Menu_AddNotifyIcon();
	Menu_MakeMenu();
	
	//Get Registry (MOVE)
	RegGetValue(m_regkey, NULL, L"MoveActive", RRF_RT_REG_BINARY, NULL, &regval, &(size = sizeof(BYTE)));
	Hook_MoveHotkeyRegister(regval);
	
	//Get Registry (CLIP)
	RegGetValue(m_regkey, NULL, L"CursorActive", RRF_RT_REG_BINARY, NULL, &regval, &(size = sizeof(BYTE)));
	Hook_ClipHotkeyRegister(regval);
	
	//Get Registry (INIT)
	Menu_SetMenuState(TN_MENU_INIT, !RegGetValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"WindowProperty", RRF_RT_REG_SZ, NULL, NULL, NULL));
	result = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\exefile\\shell\\WindowPropertyRTChecker", 0, KEY_ALL_ACCESS, &hkey);
	Menu_SetMenuState(TN_MENU_RT, result == ERROR_SUCCESS);
	if (result == ERROR_SUCCESS) {
		RegCloseKey(hkey);
	}
	
	//Show Window (Main)
	UpdateWindow(m_main);
	if (!wndHide) {
		ShowWindow(m_main, SW_SHOW);
	}
	
	LocalFree(cmdArgs);
	
	//Message Loop
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}