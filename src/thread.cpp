#include "global.hpp"

LPPTDATA t_chainstart;
LPPTDATA t_chainend;
bool t_createfirst = true;
HANDLE t_file;

//Internal

void Thread_OutputFileCreate () {
	wchar_t path[MAX_PATH];
	wchar_t file[MAX_PATH];
	SYSTEMTIME time;
	
	if (t_createfirst) {
		t_createfirst = false;
		
		GetCurrentDirectory(MAX_PATH, path);
		swprintf(file, L"%ls\\record", path);
		CreateDirectory(file, NULL);
		GetLocalTime(&time);
		swprintf(file, L"%ls\\record\\ProcessTimeRecord %04d-%02d-%02d %02d-%02d-%02d.txt", path, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		t_file = CreateFile(file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (t_file == INVALID_HANDLE_VALUE) { Util_PrintWindowsLastError(); }
	}
}

LPPTDATA Thread_CheckPID (ULONG pid) {
	LPPTDATA data = t_chainstart;
	
	do {
		if (data->pid == pid) { return data; }
	} while (data->next);
	
	return NULL;
}

LPPTDATA Thread_GetEmptyThread () {
	LPPTDATA data = t_chainstart;
	
	do {
		if (data->pid == 0) { return data; }
	} while (data->next);
	
	return NULL;
}

void Thread_WriteResult (LPPTDATA data) {
	char output[MAX_PATH];
	DWORD cnt;
	DWORD acctime = 0;
	FILETIME start, end, ft[2];
	SYSTEMTIME convert;
	
	GetProcessTimes(data->process, &start, &end, &ft[0], &ft[1]); //Data - Runtime
	
	sprintf(output, "%s(%s)\r\n", data->name, data->winname);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Path
	
	FileTimeToLocalFileTime(&start, &ft[0]);
	FileTimeToSystemTime(&ft[0], &convert);
	sprintf(output, "*Start Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Start Time
	
	FileTimeToLocalFileTime(&end, &ft[1]);
	FileTimeToSystemTime(&ft[1], &convert);
	sprintf(output, "*End Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - End Time
	
	Util_DateOperate(&end, &start);
	Util_FileTimeToTime(&end, &convert);
	sprintf(output, "*Total Time: %d%s %d%s %d%s\r\n", convert.wHour, DLG_THREAD_HOUR, convert.wMinute, DLG_THREAD_MINUTE, convert.wSecond, DLG_THREAD_SECOND);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Total Time
	
	RegGetValue(m_regrec, NULL, data->path, RRF_RT_REG_DWORD, NULL, &acctime, &(cnt = sizeof(DWORD)));
	acctime += convert.wSecond;
	acctime += convert.wMinute * 60;
	acctime += convert.wHour * 3600;
	RegSetValueEx(m_regrec, data->path, 0, REG_DWORD, (BYTE*)&acctime, sizeof(DWORD));
	
	sprintf(output, "*Accumulated Time: %d%s %d%s %d%s\r\n\r\n", acctime / 3600, DLG_THREAD_HOUR, (acctime % 3600) / 60, DLG_THREAD_MINUTE, (acctime % 3600) % 60, DLG_THREAD_SECOND);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Accumuulated Time
}

DWORD Thread_Run (LPVOID lpptdata) {
	LPPTDATA data = (LPPTDATA)lpptdata;
	
	while (true) {
		WaitForSingleObject(data->event, INFINITE);
		WaitForSingleObject(data->process, INFINITE);
		Thread_WriteResult(data);
		CloseHandle(data->process);
		data->process = NULL;
		data->pid = 0;
	}
	
	return 0;
}

//External
void Thread_CreateThread (HWND hwnd, LPCWSTR name) {
	ULONG pid = Util_GetProcessID(hwnd);
	LPPTDATA check;
	HANDLE process;
	LPPTDATA temp;
	DWORD cnt;
	wchar_t path[MAX_PATH];
	
	process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if (!process) {
		Util_PrintWindowsLastError();
		return;
	}
	
	Thread_OutputFileCreate();
	
	check = Thread_CheckPID(pid);
	if (check) {
		MessageBox(NULL, DLG_THREAD_TIME, L"Running...", MB_OK);
		return;
	}
	
	check = Thread_GetEmptyThread();
	if (!check) {
		temp = (LPPTDATA)malloc(sizeof(PTDATA));
		ZeroMemory(temp, sizeof(PTDATA));
		temp->event = CreateEvent(NULL, FALSE, FALSE, NULL);
		temp->thread = CreateThread(NULL, 0, Thread_Run, temp, 0, NULL);
		temp->process = process;
		temp->pid = pid;
		QueryFullProcessImageName(process, 0, temp->path, &cnt);
		GetWindowText(hwnd, path, 260);
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, temp->winname, 260, NULL, NULL));
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, name, -1, temp->name, 260, NULL, NULL));
		SetEvent(temp->event);
		t_chainend->next = temp;
		t_chainend = temp;
	} else {
		check->process = process;
		check->pid = pid;
		QueryFullProcessImageName(process, 0, check->path, &cnt);
		GetWindowText(hwnd, path, 260);
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, check->winname, 260, NULL, NULL));
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, name, -1, check->name, 260, NULL, NULL));
		SetEvent(check->event);
	}
}

void Thread_CreateThreadProcess (LPCWSTR filepath) {
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	LPPTDATA check;
	LPPTDATA temp;
	
	si.cb = sizeof(si);
	CreateProcess(filepath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	
	if (!pi.hProcess) {
		Util_PrintWindowsLastError();
		return;
	}
	
	Thread_OutputFileCreate();
	
	check = Thread_CheckPID(pi.dwProcessId);
	if (check) {
		MessageBox(NULL, DLG_THREAD_TIME, L"Running...", MB_OK);
		return;
	}
	
	check = Thread_GetEmptyThread();
	if (!check) {
		temp = (LPPTDATA)malloc(sizeof(PTDATA));
		ZeroMemory(temp, sizeof(PTDATA));
		temp->event = CreateEvent(NULL, FALSE, FALSE, NULL);
		temp->thread = CreateThread(NULL, 0, Thread_Run, temp, 0, NULL);
		temp->process = pi.hProcess;
		temp->pid = pi.dwProcessId;
		wcscpy(temp->path, filepath);
		strcpy(temp->winname, "Direct measurement");
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, filepath, -1, temp->name, 260, NULL, NULL));
		SetEvent(temp->event);
		t_chainend->next = temp;
		t_chainend = temp;
	} else {
		check->process = pi.hProcess;
		check->pid = pi.dwProcessId;
		wcscpy(check->path, filepath);
		strcpy(check->winname, "Direct measurement");
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, filepath, -1, check->name, 260, NULL, NULL));
		SetEvent(check->event);
	}
}

void Thread_Init () {
	t_chainstart = (LPPTDATA)malloc(sizeof(PTDATA));
	ZeroMemory(t_chainstart, sizeof(PTDATA));
	t_chainstart->event = CreateEvent(NULL, FALSE, FALSE, NULL);
	t_chainstart->thread = CreateThread(NULL, 0, Thread_Run, t_chainstart, 0, NULL);
	t_chainend = t_chainstart;
}

void Thread_Close () {
	LPPTDATA data = t_chainstart;
	
	do {
		t_chainstart = data->next;
		if (data->thread) { TerminateThread(data->thread, 0); CloseHandle(data->thread); }
		if (data->process) { CloseHandle(data->process); }
		if (data->event) { CloseHandle(data->event); }
		free(data);
		data = t_chainstart;
	} while (data);
	
	t_chainstart = NULL;
	t_chainend = NULL;
	
	CloseHandle(t_file);
	t_file = NULL;
}