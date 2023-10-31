#include "global.hpp"

//Variable

HWND m_main;
HBRUSH m_hbrush;
HINSTANCE m_hInstance;
HFONT m_font;
HKEY m_regkey;
HKEY m_regset;

//Function
#define CreateButtonMacro(h, id, cb, x, y, cx, cy) Control_CreateButton(h, BUTTON_##id##_CAPTION, BUTTON_##id##_TOOLTIP, cb, x, y, cx, cy, ID_BUTTON_##id)

void Main_Close () {
	DestroyMenu(me_menu);
	DeleteObject(m_font);
	RegCloseKey(m_regkey);
	RegCloseKey(m_regset);
	Menu_RemoveNotifyIcon();
	FreeLibrary(c_comctlModule);
	PostQuitMessage(0);
}

LRESULT CALLBACK FilterProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
	LONG_PTR style;
	
	#ifdef _DEBUG
	Debug_ConvertWindowMessage(uMsg);
	#endif
	
	WindowEventCase(uMsg) {
		WindowEvent(WM_INITDIALOG) {
			//Set Style Button
			Control_PropDialogInit(hwnd, u_filter[0], u_filter[1], true, NULL, DLG_PROP_ADD2_FILTER, false);
			
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_LBUTTONDOWN) {
			SetFocus(hwnd);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		WindowEvent(WM_COMMAND) {
			DialogEventCase(EventDialog()) {
				//Reset
				DialogEvent(ID_BUTTON_PROP_ADD2) {
					if (EventMessage() == BN_CLICKED) {
						for (int i = 0; i < 32; i++) {
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
								Button_SetCheck(tmphwnd, BST_UNCHECKED);
							}
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
								Button_SetCheck(tmphwnd, BST_UNCHECKED);
							}
						}
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_VISIBLE), BST_CHECKED);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_TW), BST_INDETERMINATE);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_NRB), BST_INDETERMINATE);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_NA), BST_INDETERMINATE);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_PW), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), BST_UNCHECKED);
					}
					break;
				}
				//Confirm/Cancel
				DialogEvent(ID_BUTTON_PROP_CONFIRM) {
					if (EventMessage() == BN_CLICKED) {
						u_filter[0][0] = 0;
						u_filter[0][1] = 0;
						u_filter[1][0] = 0;
						u_filter[1][1] = 0;
						
						for (int i = 0; i < 32; i++) {
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
								switch (Button_GetCheck(tmphwnd)) {
									case BST_CHECKED: u_filter[0][0] |= 1 << i; break;
									case BST_INDETERMINATE: u_filter[0][1] |= 1 << i; break;
								}
							}
							if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
								switch (Button_GetCheck(tmphwnd)) {
									case BST_CHECKED: u_filter[1][0] |= 1 << i; break;
									case BST_INDETERMINATE: u_filter[1][1] |= 1 << i; break;
								}
							}
						}
						RegSetValueEx(m_regset, L"FilterIncludeStyle", 0, REG_DWORD, (BYTE*)&u_filter[0][0], sizeof(DWORD));
						RegSetValueEx(m_regset, L"FilterExcludeStyle", 0, REG_DWORD, (BYTE*)&u_filter[0][1], sizeof(DWORD));
						RegSetValueEx(m_regset, L"FilterIncludeExStyle", 0, REG_DWORD, (BYTE*)&u_filter[1][0], sizeof(DWORD));
						RegSetValueEx(m_regset, L"FilterExcludeExStyle", 0, REG_DWORD, (BYTE*)&u_filter[1][1], sizeof(DWORD));
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

DWORD WINAPI HWNDDetect (LPVOID param) {
	DWORD idx = 0, len = 256, size[2];
	wchar_t name[256];
	DWORD style[2];
	HWND hwnd;
	
	while (true) {
		//Search Registry
		while (RegEnumKeyEx(m_regset, idx++, name, &len, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS) {
			//Search HWND
			if (hwnd = GetAncestor(FindWindow(name, NULL), GA_ROOTOWNER)) {
				//Not Maximized
				if (!IsMaximized(hwnd)) {
					size[0] = sizeof(style[0]);
					size[1] = sizeof(style[1]);
					if ((RegGetValue(m_regset, name, L"Style", RRF_RT_DWORD, NULL, &style[0], &size[0]) == ERROR_SUCCESS) &&
						(RegGetValue(m_regset, name, L"ExStyle", RRF_RT_DWORD, NULL, &style[1], &size[1]) == ERROR_SUCCESS)) {
						if ((GetWindowLongPtr(hwnd, GWL_STYLE) != style[0]) ||
							(GetWindowLongPtr(hwnd, GWL_EXSTYLE) != style[1])) {
							//Set Window Properties - If not same
							SetWindowLongPtr(hwnd, GWL_STYLE, style[0]);
							SetWindowLongPtr(hwnd, GWL_EXSTYLE, style[1]);
							//Set TOPMOST | VISIBLE
							SetWindowPos(hwnd, style[1] & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST,
										 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
						}
					}
				}
			}
			len = 256;
		}
		idx = 0;
		Sleep(500);
	}
	
	return 0;
}

LRESULT CALLBACK MainProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmphwnd;
	wchar_t pidhwnd[10];
	wchar_t text[4];
	wchar_t name[30];
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
			//Prevent List-View Header Control
			if (ListViewMessage() == HDN_BEGINTRACK)
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
					
					Control_SetChangeText(tmphwnd, GetDlgItem(hwnd, ID_BUTTON_CAPTURE));
					swprintf(text, L"%d", changed ? (int)ceil(alpha / 255.0 * 100.0) : 100);
					SetDlgItemText(hwnd, ID_EDIT_ALPHA, text);
				}
				if (ListViewMessage() == NM_CUSTOMDRAW) {
					LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
					wchar_t cls[256];
					HKEY key;
					
					switch (lplvcd->nmcd.dwDrawStage) {
						case CDDS_PREPAINT: return CDRF_NOTIFYITEMDRAW;
						case CDDS_ITEMPREPAINT: {
							ListView_GetItemText(lplvcd->nmcd.hdr.hwndFrom, lplvcd->nmcd.dwItemSpec, 3, pidhwnd, 10);
							tmphwnd = (HWND)_wtoi(pidhwnd);
							
							GetClassName(tmphwnd, cls, 256);
							if (RegOpenKeyEx(m_regset, cls, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
								lplvcd->clrText = RGB(255,0,0);
								RegCloseKey(key);
							}
							break;
						}
					}
				}
			}
			break;
		}
		WindowEvent(WM_SYSCOMMAND) {
			if (wParam != SC_RESTORE) {
				break;
			}
		}
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
				DialogEvent(ID_BUTTON_HOTKEY)
				DialogEvent(ID_BUTTON_FILTER) {
					if (EventMessage() == BN_CLICKED) {
						//Change Window Property
						void (*executionFunc)(HWND, HWND, LPCWSTR) = NULL;
						
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
							case ID_BUTTON_HOTKEY:
								if(!IsWindowVisible(h_window)) {
									//Set Hotkey Window Position
									GetWindowRect(m_main, &rect);
									SetWindowPos(h_window, HWND_TOPMOST, rect.left + 20, rect.top + 20, 0, 0, SWP_NOSIZE);
								}
								//Show Hotkey Window
								ShowWindow(h_window, SW_SHOW);
								break;
							case ID_BUTTON_FILTER:
								if (!DialogBox(m_hInstance, MAKEINTRESOURCE(ID_DLG_PROP), hwnd, FilterProc)) {
									Log_Message(LOG_CHANGE_FILTER, NULL);
								}
								Control_RefreshListView();
								break;
						}
						
						if (executionFunc == NULL) { break; }
						ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 0, name, 30);
						if (name[0] == L'*') { wcscpy(name, name + 1); }
						
						//Selected Process Execution
						ListView_GetItemText(GetDlgItem(hwnd, ID_LIST), c_listViewIndex, 3, pidhwnd, 10);
						if (IsWindow((HWND)_wtoi(pidhwnd))) {
							executionFunc((HWND)_wtoi(pidhwnd), (HWND)lParam, name);
						} else {
							Log_Message(LOG_NO_WINDOW, (HWND)_wtoi(pidhwnd), name);
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
			if (wParam == h_moveHotkey) {
				Hook_MouseHook();
				
				#ifdef _DEBUG
				puts("Hotkey Pressed: Move Active");
				#endif
				break;
			}
			if (wParam == h_clipHotkey) {
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

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	BYTE regval;
	DWORD size;
	WNDCLASSEX wc = {};
	
	//Duplicate Prevent
	if (FindWindow(WINDOW_MAIN_NAME, NULL) != NULL) {
		ShowWindow(FindWindow(WINDOW_MAIN_NAME, NULL), SW_RESTORE);
		SetForegroundWindow(FindWindow(WINDOW_MAIN_NAME, NULL));
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
							CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
							NULL, NULL, hInstance, NULL);
	Util_CheckError(m_main);
	
	//Create Log/Hotkey Window
	Log_CreateWindow(m_main);
	Hook_CreateWindow(m_main);
	
	//Check Hotkey
	if (Util_GetHotkey(HOTKEY_MOVE) == 0) {
		Util_SetHotkey(HOTKEY_MOVE, VK_F3);
	}
	if (Util_GetHotkey(HOTKEY_CURSOR) == 0) {
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
	
	//Create Button (MOVE)
	CreateButtonMacro(m_main, MOVE, true, 475, 390, 100, 30);
	
	//Create Button (CLIP)
	CreateButtonMacro(m_main, CLIP, true, 475, 430, 100, 30);
	
	//Create List-View
	Control_CreateListView(m_main, LIST_TOOLTIP, 10, 10, 450, 440, ID_LIST);
	Control_RefreshListView();
	
	//Add System Tray Notify Icon
	Menu_RemoveNotifyIcon();
	Menu_AddNotifyIcon(m_main);
	Menu_MakeMenu();
	
	//Get Registry (MOVE)
	size = sizeof(BYTE);
	
	RegGetValue(m_regkey, NULL, L"MoveActive", RRF_RT_REG_BINARY, NULL, &regval, &size);
	Hook_MoveHotkeyRegister(regval);
	
	//Get Registry (CLIP)
	size = sizeof(BYTE);
	
	RegGetValue(m_regkey, NULL, L"CursorActive", RRF_RT_REG_BINARY, NULL, &regval, &size);
	Hook_ClipHotkeyRegister(regval);
	
	//Get Registry (INIT)
	Menu_SetMenuState(TN_MENU_INIT, !RegGetValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"WindowProperty", RRF_RT_REG_SZ, NULL, NULL, NULL));
	
	//Show Window (Main)
	UpdateWindow(m_main);
	if (strcmp(pCmdLine, "-hide")) {
		ShowWindow(m_main, SW_SHOW);
	}
	
	//Create Thread (Setting)
	CreateThread(NULL, 0, HWNDDetect, NULL, 0, NULL);
	
	//Message Loop
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}