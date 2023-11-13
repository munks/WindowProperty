#include "global.hpp"

//Variable

HWND l_window;
HWND l_logmsg;

//Internal

void AddTextToLogEdit (LPCWSTR message) {
	int lastPos = Edit_GetTextLength(l_logmsg);
	
	Edit_SetSel(l_logmsg, lastPos, lastPos);
	Edit_ReplaceSel(l_logmsg, message);
}

LRESULT CALLBACK LogProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

void Log_Message (LPCWSTR format, LPCWSTR msg1, LPCWSTR msg2, LPCWSTR msg3) {
	wchar_t output[100];
	time_t t;
	wchar_t timeText[15];
	
	time(&t);
	wcsftime(timeText, 15, L"%H:%M:%S - ", localtime(&t));
	swprintf(output, format, msg1, msg2, msg3);
	
	AddTextToLogEdit(timeText);
	AddTextToLogEdit(output);
	AddTextToLogEdit(L"\r\n");
}

void Log_CreateWindow (HWND main) {
	//Create Window Class (Log)
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = LogProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = WINDOW_LOG_NAME;
	wc.hbrBackground = m_hbrush;
	wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));

	Util_CheckError((void*)MAKELONG(RegisterClassEx(&wc), 0));
	
	//Create Log Window
	l_window = CreateWindowEx(WS_EX_TOPMOST, WINDOW_LOG_NAME, WINDOW_LOG_CAPTION,
							  WS_POPUP | WS_CAPTION | WS_SYSMENU,
							  CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
							  main, NULL, m_hInstance, NULL);
	Util_CheckError(l_window);
	
	//Create Log Message Box
	l_logmsg = CreateWindow(L"EDIT", NULL,
							WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
							ES_MULTILINE | ES_READONLY,
							10,10,370,250,
							l_window, (HMENU)ID_EDIT_LOG, m_hInstance, NULL);
	
	SetWindowFont(l_logmsg, m_font, FALSE);
	Util_CheckError(l_logmsg);
}