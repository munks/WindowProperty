#include "global.hpp"

//Variable

HHOOK h_hookActive = 0; //Active (Keyboard)
HHOOK h_hookMove = 0; //Move (Mouse)
POINT h_handlePoint;
POINT h_mousePoint;
HWND h_moveHandle = 0;

//Internal

LRESULT CALLBACK MoveHook (int nCode, WPARAM wParam, LPARAM lParam) {
	RECT rect;
	POINT point;
	
	switch (wParam) {
		case WM_LBUTTONDOWN: {
			h_mousePoint = ((LPMSLLHOOKSTRUCT)lParam)->pt;
			h_moveHandle = GetAncestor(WindowFromPoint(h_mousePoint), GA_ROOTOWNER);
			GetWindowRect(h_moveHandle, &rect);
			h_handlePoint = {rect.left, rect.top};
			return 1;
		}
		case WM_LBUTTONUP: {
			h_moveHandle = 0;
			return 1;
		}
		case WM_MOUSEMOVE: {
			if (h_moveHandle) {
				point = ((LPMSLLHOOKSTRUCT)lParam)->pt;
				SetWindowPos(h_moveHandle, HWND_TOP, h_handlePoint.x - (h_mousePoint.x - point.x), h_handlePoint.y - (h_mousePoint.y - point.y),
							0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
				h_mousePoint = point;
				GetWindowRect(h_moveHandle, &rect);
				h_handlePoint = {rect.left, rect.top};
			}
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MoveHookActive (int nCode, WPARAM wParam, LPARAM lParam) {
	if (((KBDLLHOOKSTRUCT *)lParam)->vkCode == VK_F3) {
		if (wParam == WM_KEYUP) {
			if (!h_hookMove) {
				h_hookMove = SetWindowsHookEx(WH_MOUSE_LL, MoveHook, NULL, 0);
				printf("HookMove Active: %p\n", h_hookMove);
			} else {
				UnhookWindowsHookEx(h_hookMove);
				printf("HookMove Deactive: %p\n", h_hookMove);
				h_hookMove = 0;
			}
		}
		return 1;
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//External

void Hook_MoveCallbackAttach () {
	if (h_hookActive) { return; }
	h_hookActive = SetWindowsHookEx(WH_KEYBOARD_LL, MoveHookActive, NULL, 0);
	#ifdef _DEBUG
	printf("HookActive: %p\n", h_hookActive);
	#endif
}

void Hook_MoveCallbackDetach () {
	if (!h_hookActive) { return; }
	if (h_hookMove) { UnhookWindowsHookEx(h_hookMove); }
	UnhookWindowsHookEx(h_hookActive);
	h_hookActive = 0;
	#ifdef _DEBUG
	printf("HookDeactive: %p\n", h_hookActive);
	#endif
}
