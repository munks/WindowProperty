#include "global.hpp"

//Variable

HMENU me_menu;

MENUITEMINFO 		me_mi = {sizeof(MENUITEMINFO)};
NOTIFYICONDATA 		me_nid = {};
int 				me_infoCnt = 0;
UINT_PTR 			me_timer = 0;

//Internal

#define CHECK_HR(expr) do {hr = (expr); if (FAILED(hr)) goto Final; } while (0)
#define CHECK_HR_REMOVE(expr) do {hr = (expr); if (FAILED(hr)) goto Failed; } while (0)

bool Menu_SetStartup (BOOL add) {
	HRESULT hr;
	bool	rtn = false;
	CComPtr<ITaskService> 		tsvc;
	CComPtr<ITaskFolder> 		tf;
	CComPtr<ITaskDefinition> 	td;
	CComPtr<IRegistrationInfo> 	tri;
	CComPtr<IPrincipal> 		tp;
	CComPtr<ITaskSettings> 		ts;
	CComPtr<ITriggerCollection> ttc;
	CComPtr<ITrigger> 			tt;
	CComPtr<IActionCollection> 	tac;
	CComPtr<IAction> 			ta;
	CComPtr<IExecAction> 		tea;
	CComPtr<IRegisteredTask> 	tr;
	wchar_t				path[MAX_PATH] = {};

	CHECK_HR(tsvc.CoCreateInstance(CLSID_TaskScheduler));

	CHECK_HR(tsvc->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()));

	CHECK_HR(tsvc->GetFolder(_bstr_t(L"\\"), &tf));

	if (!add) { goto Failed; }
	CHECK_HR_REMOVE(tsvc->NewTask(0, &td));

	CHECK_HR_REMOVE(td->get_RegistrationInfo(&tri));
	tri->put_Author(_bstr_t(L"Duality"));

	CHECK_HR_REMOVE(td->get_Principal(&tp));
	tp->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	tp->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);

	CHECK_HR_REMOVE(td->get_Settings(&ts));
	ts->put_StartWhenAvailable(VARIANT_TRUE);
	ts->put_DisallowStartIfOnBatteries(VARIANT_TRUE);
	ts->put_StopIfGoingOnBatteries(VARIANT_TRUE);

	CHECK_HR_REMOVE(td->get_Triggers(&ttc));
	ttc->Create(TASK_TRIGGER_LOGON, &tt);

	CHECK_HR_REMOVE(td->get_Actions(&tac));
	tac->Create(TASK_ACTION_EXEC, &ta);

	CHECK_HR_REMOVE(ta->QueryInterface(IID_IExecAction, (LPVOID*)&tea));
	GetModuleFileName(NULL, path, MAX_PATH);
	tea->put_Path(_bstr_t(path));
	tea->put_Arguments(_bstr_t(L"-hide"));

	CHECK_HR_REMOVE(tf->RegisterTaskDefinition(
		_bstr_t(L"WindowProperty"),
		td,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(L""),
		&tr
	));

	return true;

	Failed:
	rtn = SUCCEEDED(tf->DeleteTask(_bstr_t(L"WindowProperty"), 0));
	Final:
	return rtn;
}

void Menu_SetRTContext (BOOL add) {
	HKEY tmpkey;
	HKEY tmpkey2;
	wchar_t path[MAX_PATH];
	wchar_t regval[MAX_PATH];
	
	GetModuleFileName(NULL, path, MAX_PATH);
	
	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\exefile\\shell\\WindowPropertyRTChecker", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tmpkey, NULL);
	if (!add) {
		RegDeleteTree(tmpkey, NULL);
		RegCloseKey(tmpkey);
		RegDeleteKey(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\exefile\\shell\\WindowPropertyRTChecker");
	} else {
		wcscpy(regval, BUTTON_RUNTIME_CAPTION);
		RegSetValueEx(tmpkey, NULL, 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		swprintf(regval, L"\"%ls\",0", path);
		RegSetValueEx(tmpkey, L"Icon", 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		RegCreateKeyEx(tmpkey, L"command", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tmpkey2, NULL);
		swprintf(regval, L"\"%ls\" -rtcheck \"%%1\"", path);
		RegSetValueEx(tmpkey2, NULL, 0, REG_SZ, (BYTE*)regval, (wcslen(regval) + 1) * 2);
		RegCloseKey(tmpkey);
		RegCloseKey(tmpkey2);
	}
}

void Menu_DeleteNotifyIcon (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	//Remove Info
	ZeroMemory(me_nid.szInfo, sizeof(me_nid.szInfo));
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	KillTimer(NULL, me_timer);
	me_timer = 0;
}

//External

bool Menu_TaskSchedulerInit () {
	HRESULT hr;
	CComPtr<ITaskService> 		tsvc;
	CComPtr<ITaskFolder> 		tf;
	CComPtr<IRegisteredTask> 	tr;

	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) { return false; }
	
	CoInitializeSecurity(nullptr, -1 nullptr, nullptr, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, 0, nullptr);

	hr = tsvc.CoCreateInstance(CLSID_TaskScheduler);
	if (FAILED(hr)) { return false; }

	tsvc->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) { return false; }
	
	tsvc->GetFolder(_bstr_t(L"\\"), &tf);

	tf->GetTask(_bstr_t(L"WindowProperty"), &tr);
	Menu_SetMenuState(TN_MENU_INIT, SUCCEEDED(hr));

	return true;
}

void Menu_SetMenuState (WORD message, BOOL on) {
	me_mi.fMask = MIIM_STATE;
	GetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
	if (on) {
		me_mi.fState |= MFS_CHECKED;
	} else {
		me_mi.fState &= ~MFS_CHECKED;
	}
	SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
}

void Menu_SetMenuString (WORD message, LPWSTR str) {
	me_mi.fMask = MIIM_STRING;
	me_mi.dwTypeData = str;
	SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
}

void Menu_ExecuteNotifyEvent (WORD message) {
	RECT rect;
	BOOL changed;
	
	switch (message) {
		case TN_MENU_CLOSE: {
			Main_Close();
			return;
		}
		case TN_MENU_LOG: {
			if(!IsWindowVisible(l_window)) {
				//Set Log Window Position
				GetWindowRect(m_main, &rect);
				SetWindowPos(l_window, HWND_TOPMOST, rect.left + 20, rect.top + 20, 0, 0, SWP_NOSIZE);
			}
			//Show Log Window
			ShowWindow(l_window, SW_SHOW);
			break;
		}
		case TN_MENU_MOVE:
		case TN_MENU_CLIP:
		case TN_MENU_INIT:
		case TN_MENU_RT: {
			me_mi.fMask = MIIM_STATE;
			GetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
			me_mi.fState ^= MFS_CHECKED;
			SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false, &me_mi);
			changed = ((me_mi.fState & MFS_CHECKED) == MFS_CHECKED) ? BST_CHECKED : BST_UNCHECKED;
			switch (message) {
				case TN_MENU_MOVE:
					Hook_MoveHotkeyRegister(changed);
					break;
				case TN_MENU_CLIP:
					Hook_ClipHotkeyRegister(changed);
					break;
				case TN_MENU_INIT:
					me_mi.fState = Menu_SetStartup(changed) ? MFS_CHECKED : MFS_UNCHECKED;
					SetMenuItemInfo(me_menu, MAKELONG(ID_BUTTON_ICON, message), false &me_mi);
					break;
				case TN_MENU_RT:
					Menu_SetRTContext(changed);
					break;
			}
			break;
		}
	}
}

void Menu_AddNotifyIcon () {
	me_nid.cbSize = sizeof(NOTIFYICONDATA);
	me_nid.hWnd = m_main;
	me_nid.uID = 0;
	me_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	me_nid.uCallbackMessage = WM_ICONNOTIFY;
	wcscpy(me_nid.szTip, WINDOW_MAIN_CAPTION);
	me_nid.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(ID_ICON));
	
	Shell_NotifyIcon(NIM_ADD, &me_nid);
}

void Menu_RemoveNotifyIcon () {
	me_nid.hWnd = m_main;
	me_nid.uID = 0;
	Shell_NotifyIcon(NIM_DELETE, &me_nid);
}

void Menu_InfoNotifyIcon (LPCWSTR title, LPCWSTR info, int len) {
	//Log
	Log_Message(LOG_FORMAT_NORMAL, title, info, NULL);
	
	//Init
	me_infoCnt++;
	me_nid.uFlags = NIF_INFO;
	me_nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	
	//Add Info
	wcscpy(me_nid.szInfoTitle, title);
	wcscpy(me_nid.szInfo, info);
	
	Shell_NotifyIcon(NIM_MODIFY, &me_nid);
	
	//Wait
	if (me_timer) {
		KillTimer(NULL, me_timer);
	}
	me_timer = SetTimer(NULL, NOTIFY_DELETE, len, Menu_DeleteNotifyIcon);
}

void Menu_MakeMenu () {
	me_menu = CreatePopupMenu();
	
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_MOVE), MENU_MOVE_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLIP), MENU_CLIP_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_INIT), MENU_START_TEXT);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_RT), MENU_RUNTIME_TEXT);
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_LOG), MENU_LOG_TEXT);
	AppendMenu(me_menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(me_menu, MF_STRING | MF_UNCHECKED, MAKELONG(ID_BUTTON_ICON, TN_MENU_CLOSE), MENU_CLOSE_TEXT);
}