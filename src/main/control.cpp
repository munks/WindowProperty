#include "global.hpp"

//Variable

HMODULE c_comctlModule;
ILD IL_Destroy;
ILC IL_Create;
ILRI IL_ReplaceIcon;

HIMAGELIST c_imageList;

HWND c_listView;
int c_listViewIndex;

LVINFO c_listViewInfo[LV_MAX_COL] = {
	{L"Process Name", 0.29},
	{L"Process Title", 0.42},
	{L"PID", 0.1},
	{L"HWND", 0.15}
};

//Internal

void Control_CreateTooltip (HWND hwnd, HWND hitem, LPCWSTR tooltip) {
	HWND tmphwnd;
	wchar_t txt[MAX_PATH];
	TOOLINFO ti = { 0 };
	
	tmphwnd = CreateWindow(TOOLTIPS_CLASS, NULL,
							WS_POPUP | TTS_ALWAYSTIP,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							hwnd, NULL, 
							m_hInstance, NULL);
	Util_CheckError(tmphwnd);
	
	wcscpy(txt, tooltip);
	
	ti.cbSize = sizeof(ti) - sizeof(void*);
	ti.hwnd = hwnd;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.uId = (UINT_PTR)hitem;
	ti.lpszText = txt;
	
	SendMessage(tmphwnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

//External

void Control_InitDLL () {
	c_comctlModule = LoadLibraryA("comctl32.dll");
	
	IL_Destroy = (ILD)GetProcAddress(c_comctlModule, "ImageList_Destroy");
	IL_Create = (ILC)GetProcAddress(c_comctlModule, "ImageList_Create");
	IL_ReplaceIcon = (ILRI)GetProcAddress(c_comctlModule, "ImageList_ReplaceIcon");
}

HWND Control_CreateButton (HWND hwnd, LPCWSTR caption, LPCWSTR tooltip, bool checkbox, int x, int y, int w, int h, int idtf) {
	HWND tmphwnd;
	
	tmphwnd = CreateWindow(L"BUTTON", caption,
							WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP |
							BS_CENTER | BS_VCENTER | checkbox * BS_AUTOCHECKBOX,
							x, y, w, h,
							hwnd, (HMENU)idtf, m_hInstance, NULL);
	Util_CheckError(tmphwnd);
	
	#ifdef _DEBUG
	wprintf(L"CreateButton: %ls\n", caption);
	#endif
	
	if (wcslen(tooltip) != 0) {
		Control_CreateTooltip(hwnd, tmphwnd, tooltip);
	}
	
	SetWindowFont(tmphwnd, m_font, FALSE);
	
	return tmphwnd;
}

HWND Control_CreateEdit (HWND hwnd, LPCWSTR tooltip, int x, int y, int w, int h, int idtf, LPCWSTR txt) {
	HWND tmphwnd;
	
	tmphwnd = CreateWindow(L"EDIT", NULL,
							WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER,
							x, y, w, h,
							hwnd, (HMENU)idtf, m_hInstance, NULL);
	Util_CheckError(tmphwnd);
	
	#ifdef _DEBUG
	wprintf(L"CreateEdit: %ls\n", txt);
	#endif
	
	if (wcslen(tooltip) != 0) {
		Control_CreateTooltip(hwnd, tmphwnd, tooltip);
	}
	
	SetWindowFont(tmphwnd, m_font, FALSE);
	Edit_SetText(tmphwnd, txt);
	
	return tmphwnd;
}

HWND Control_CreateStatic (HWND hwnd, int x, int y, int w, int h, int idtf, LPCWSTR txt) {
	HWND tmphwnd;
	
	tmphwnd = CreateWindow(L"STATIC", NULL,
							WS_CHILD | WS_VISIBLE | SS_CENTER,
							x, y, w, h,
							hwnd, (HMENU)idtf, m_hInstance, NULL);
	Util_CheckError(tmphwnd);
	
	#ifdef _DEBUG
	wprintf(L"CreateStatic: %ls\n", txt);
	#endif
	
	SetWindowFont(tmphwnd, m_font, FALSE);
	Static_SetText(tmphwnd, txt);
	
	return tmphwnd;
}

void Control_CreateListView (HWND hwnd, LPCWSTR tooltip, int x, int y, int w, int h, int idtf) {
	LVCOLUMNW col = {0, };
	
	//Make Control
	c_listView = CreateWindow(WC_LISTVIEWW, NULL,
							LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS |
							WS_CHILD | WS_VISIBLE | WS_BORDER,
							x, y, w, h,
							hwnd, (HMENU)idtf, m_hInstance, NULL);
	Util_CheckError(c_listView);
	
	#ifdef _DEBUG
	_putws(L"CreateList");
	#endif
	
	if (wcslen(tooltip) != 0) {
		Control_CreateTooltip(hwnd, c_listView, tooltip);
	}
	
	SendMessage(c_listView, CCM_SETVERSION, 5, 0);
	SetWindowFont(c_listView, m_font, FALSE);
	
	//Init List-View Header
	ListView_SetExtendedListViewStyle(c_listView, LVS_EX_FULLROWSELECT);
	col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	
	_repeat (i, LV_MAX_COL) {
		col.cx = c_listViewInfo[i].ratio * w;
		col.pszText = c_listViewInfo[i].text;
		ListView_InsertColumn(c_listView, i, &col);
	}
}

void Control_RefreshListView () {
	wchar_t outputText[MAX_PATH];
	LVITEM item;
	HICON icon;
	int idx;
	
	IL_Destroy(c_imageList);
	c_imageList = IL_Create(16, 16, ILC_MASK | ILC_COLOR32, 1, 1);
	idx = ListView_GetTopIndex(c_listView);
	
	//Clear List
	ListView_DeleteAllItems(c_listView);
	ZeroMemory(&item, sizeof(item));
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.iItem = 0;
	item.iImage = 0;
	
	//Loop Macro (while)
	ProcessLoop {
		WindowLoop(ProcessLoopID()) {
			if (Util_WindowFilter(WindowLoopHandle())) {
				icon = (HICON)GetClassLongPtr(WindowLoopHandle(), GCLP_HICON);
				IL_ReplaceIcon(c_imageList, -1, icon ? icon : LoadIcon(NULL, IDI_APPLICATION));
				outputText[0] = L'\0';
				_repeat (i, LV_MAX_COL) {
					switch (i) {
						case 0: { //Item 0(Process Name)
							if ((GetWindowLong(WindowLoopHandle(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0) {
								wcscat(outputText, L"*"); //Add Asterisk If HWND is TOPMOST
							}
							wcscat(outputText, ProcessLoopName());
							break;
						}
						case 1: { //Item 1(Process Title)
							GetWindowText(WindowLoopHandle(), outputText, MAX_PATH);
							break;
						}
						case 2: { //Item 2(Process Id)
							swprintf(outputText, L"%d", ProcessLoopID());
							break;
						}
						case 3: { //Item 3(Window Handle)
							swprintf(outputText, L"%d", WindowLoopHandle());
							break;
						}
					}
					item.iSubItem = i;
					item.pszText = outputText;
					if (i == 0) {
						ListView_InsertItem(c_listView, &item);
					} else {
						ListView_SetItem(c_listView, &item);
					}
				}

				//Next
				item.iItem++;
				item.iImage++;
			}
		} WindowLoopEnd();
	} ProcessLoopEnd();
	
	//Program Will Be Crash If List Item Not Selected
	if (c_listViewIndex < 0 || c_listViewIndex > item.iItem) {
		c_listViewIndex = 0;
	}
	
	ListView_SetItemState(c_listView, c_listViewIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_SetImageList(c_listView, c_imageList, LVSIL_SMALL);
	SetFocus(c_listView);
	ListView_Scroll(c_listView, 0, 17 * idx);
}