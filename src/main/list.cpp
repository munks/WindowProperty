#include "global.hpp"

//Variable

HWND li_window;
HWND li_edit;

//Internal

void List_ShowWindow (LPCWSTR caption) {
	SetWindowText(li_window, caption);
	ShowWindow(li_window, SW_SHOW);
}

void List_AddText (LPCWSTR message) {
	int lastPos = Edit_GetTextLength(li_edit);
	
	Edit_SetSel(li_edit, lastPos, lastPos);
	Edit_ReplaceSel(li_edit, message);
}

void List_RemoveText () {
	Edit_SetText(li_edit, L"");
}

LRESULT CALLBACK ListProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WindowEventCase(uMsg) {
		WindowEvent(WM_SYSCOMMAND) {
			if (wParam == SC_CLOSE) {
				ShowWindow(hwnd, SW_HIDE);
				return 0;
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//External

void List_CreateWindow (HWND main) {
	//Create Window Class (List)
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = ListProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = WINDOW_LIST_NAME;
	wc.hbrBackground = m_hbrush;
	wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));

	Util_CheckError((void*)MAKELONG(RegisterClassEx(&wc), 0));
	
	//Create List Window
	li_window = CreateWindowEx(WS_EX_TOPMOST, WINDOW_LIST_NAME, WINDOW_LIST_CAPTION,
							  WS_POPUP | WS_CAPTION | WS_SYSMENU,
							  CW_USEDEFAULT, CW_USEDEFAULT, 600, 300,
							  main, NULL, m_hInstance, NULL);
	Util_CheckError(li_window);
	
	//Create List Message Box
	li_edit = CreateWindow(L"EDIT", NULL,
							WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
							ES_MULTILINE | ES_READONLY,
							10,10,570,250,
							li_window, (HMENU)ID_EDIT_LIST, m_hInstance, NULL);
	SetWindowFont(li_edit, m_font, FALSE);
	Util_CheckError(li_edit);
}