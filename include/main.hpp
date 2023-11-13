#pragma once

//Macro - Case Switch
#define WindowEventCase(c)	switch(c)
#define DialogEventCase(c)	switch(c)
#define NotifyEventCase(c)	switch(c)
#define WindowEvent(e)		case e:
#define DialogEvent(e)		case e:
#define NotifyEvent(e)		case e:

//Macro - Useful Combine
#define EventDialog()		LOWORD(wParam)
#define EventMessage()		HIWORD(wParam)
#define ListViewMessage()	((LPNMHDR)lParam)->code
#define ListViewDialog()	((LPNMHDR)lParam)->hwndFrom

void Main_Close ();

extern HWND m_main;
extern HBRUSH m_hbrush;
extern HINSTANCE m_hInstance;
extern HFONT m_font;
extern HKEY m_regkey;
extern HKEY m_regset;
extern bool m_loopStop;

typedef struct HDATA {
	wchar_t** name;
	wchar_t** cls;
	DWORD total;
} HDATA;