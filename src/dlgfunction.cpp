#include "global.hpp"

//Internal

int DlgFunction_CheckButtonState (HWND hwnd, int init, ...) {
	int state[3] = {};
	int cnt = 0;
	int val;
	
	for (int idx = 1;; idx++) {
		val = *(&init + ((sizeof(size_t) / sizeof(int)) * idx));
		if (val == -1) { break; }
		state[Button_GetCheck(GetDlgItem(hwnd, init + val))]++;
		cnt++;
	}
	
	if (state[BST_UNCHECKED] == cnt) {
		return BST_UNCHECKED;
	} else if (state[BST_CHECKED] == cnt) {
		return BST_CHECKED;
	} else if (state[BST_INDETERMINATE] == cnt) {
		return BST_INDETERMINATE;
	}
	
	return BST_UNCHECKED;
}

void DlgFunction_ButtonSetType (HWND btn, LONG_PTR type) {
	LONG_PTR style = GetWindowLongPtr(btn, GWL_STYLE);
	
	style &= ~0x000F;
	SetWindowLongPtr(btn, GWL_STYLE, style | type);
}

//External

void DlgFunction_ResetFilter (HWND hwnd) {
	HWND tmphwnd;
	
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

void DlgFunction_SetProperties (HWND hwnd, PLONG_PTR style, PLONG_PTR exstyle) {
	LONG_PTR prop[2] = {0};
	HWND tmphwnd;
	
	for (int i = 0; i < 32; i++) {
		if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
			if (Button_GetCheck(tmphwnd) == BST_CHECKED) {
				prop[0] |= 1 << i;
			}
		}
		if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
			if (Button_GetCheck(tmphwnd) == BST_CHECKED) {
				prop[1] |= 1 << i;
			}
		}
	}
	
	*style = prop[0];
	*exstyle = prop[1];
}

void DlgFunction_SetFilter (HWND hwnd) {
	HWND tmphwnd;
	
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
}

void DlgFunction_PropInit (HWND hwnd, LONG_PTR style[], LONG_PTR exstyle[], bool exclude, LPCWSTR add, bool runtime) {
	HWND tmphwnd;
	LONG_PTR prop;
	
	SetWindowText(hwnd, DLG_PROP_TITLE);
	
	SetWindowText(GetDlgItem(hwnd, ID_STATIC_STYLE), DLG_PROP_LINK_STYLE);
	SetWindowText(GetDlgItem(hwnd, ID_STATIC_EXSTYLE), DLG_PROP_LINK_EXSTYLE);
	
	add ? SetWindowText(GetDlgItem(hwnd, ID_BUTTON_PROP_ADD), add) : ShowWindow(GetDlgItem(hwnd, ID_BUTTON_PROP_ADD), SW_HIDE);
	runtime ? SetWindowText(GetDlgItem(hwnd, ID_STATIC_TIME), DLG_PROP_TIME) : ShowWindow(GetDlgItem(hwnd, ID_STATIC_TIME), SW_HIDE);
	
	SetWindowText(GetDlgItem(hwnd, ID_BUTTON_PROP_CONFIRM), DLG_PROP_CONFIRM);
	SetWindowText(GetDlgItem(hwnd, ID_BUTTON_PROP_CANCEL), DLG_PROP_CANCEL);
	
	for (int i = 0; i < 32; i++) {
		if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON + i)) != NULL) {
			prop = (style[0] & (1 << i)) ? BST_CHECKED : BST_UNCHECKED;
			if (exclude) {
				DlgFunction_ButtonSetType(tmphwnd, BS_AUTO3STATE);
				prop = (style[1] & (1 << i)) ? BST_INDETERMINATE : prop;
			}
			Button_SetCheck(tmphwnd, prop);
		}
		if ((tmphwnd = GetDlgItem(hwnd, PROP_BUTTON_EX + i)) != NULL) {
			prop = (exstyle[0] & (1 << i)) ? BST_CHECKED : BST_UNCHECKED;
			if (exclude) {
				DlgFunction_ButtonSetType(tmphwnd, BS_AUTO3STATE);
				prop = (exstyle[1] & (1 << i)) ? BST_INDETERMINATE : prop;
			}
			Button_SetCheck(tmphwnd, prop);
		}
	}
	if (exclude) {
		DlgFunction_ButtonSetType(GetDlgItem(hwnd, ID_BUTTON_STL_PW), BS_AUTO3STATE);
		DlgFunction_ButtonSetType(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), BS_AUTO3STATE);
		DlgFunction_ButtonSetType(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), BS_AUTO3STATE);
		DlgFunction_ButtonSetType(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), BS_AUTO3STATE);
		DlgFunction_ButtonSetType(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), BS_AUTO3STATE);
	}
	
	//POPUPWINDOW
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_PW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 19, 23, 31, -1));
	//CAPTION
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 22, 23, -1));
	//OVERLAPPEDWINDOW
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 16, 17, 18, 19, 34, -1));
	//PALETTEWINDOW (Extended)
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON_EX, 3, 7, 8, -1));
	//OVERLAPPEDWINDOW (Extended)
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON_EX, 8, 9, -1));
}

void DlgFunction_PropButtonState (HWND hwnd, int id, HWND ctrl) {
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
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_PW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 19, 23, 31, -1));
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_CAPTION), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 22, 23, -1));
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_STL_OLW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON, 16, 17, 18, 19, 34, -1));
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_PW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON_EX, 3, 7, 8, -1));
	Button_SetCheck(GetDlgItem(hwnd, ID_BUTTON_EXSTL_OLW), DlgFunction_CheckButtonState(hwnd, PROP_BUTTON_EX, 8, 9, -1));
}

DWORD WINAPI DlgFunction_SystemTimeLoop (LPVOID param) {
	FILETIME ftCurrent;
	FILETIME ftOrigin;
	HWND check;
	HWND display;
	SYSTEMTIME calctime;
	wchar_t timetext[260];
	
	ftOrigin = ((TDATA*)param)->ft;
	check = ((TDATA*)param)->hwnd;
	display = ((TDATA*)param)->timer;
	
	while (check) {
		GetSystemTimeAsFileTime(&ftCurrent);
		Util_DateOperate(&ftCurrent, &ftOrigin);
		Util_FileTimeToTime(&ftCurrent, &calctime);
		swprintf(timetext, DLG_PROP_TIME, calctime.wHour, calctime.wMinute, calctime.wSecond, calctime.wMilliseconds);
		SetWindowText(display, timetext);
		Sleep(125);
	}
	
	return 0;
}

bool DlgFunction_HotkeyValidCheck (HWND hwnd, int id1, int id2) {
	int check = 0;
	
	if (SendDlgItemMessage(hwnd, id1, HKM_GETHOTKEY, 0, 0)) {
		check |= (1 << 0);
	}
	if (SendDlgItemMessage(hwnd, id2, HKM_GETHOTKEY, 0, 0)) {
		check |= (1 << 1);
	}
	
	return (check == ((1 << 0) | (1 << 1)));
}