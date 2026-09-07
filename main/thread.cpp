#include "global.hpp"

LPPTDATA t_chainstart;
LPPTDATA t_chainend;
bool t_createfirst = true;
HANDLE t_file;

//Internal

static bool Thread_OutputFileCreate () {
	wchar_t path[MAX_PATH];
	wchar_t file[MAX_PATH];
	SYSTEMTIME time;
	
	if (t_createfirst) {
		GetCurrentDirectory(MAX_PATH, path);
		swprintf_s(file, ARRAYSIZE(file), L"%ls\\record", path);
		if (!CreateDirectory(file, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
			Util_PrintWindowsLastError();
			return false;
		}
		GetLocalTime(&time);
		swprintf_s(file, ARRAYSIZE(file), L"%ls\\record\\ProcessTimeRecord %04d-%02d-%02d %02d-%02d-%02d.txt", path, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		t_file = CreateFile(file, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (t_file == INVALID_HANDLE_VALUE) {
			Util_PrintWindowsLastError();
			t_file = nullptr;
			return false;
		}
		t_createfirst = false;
	}

	return true;
}

static LPPTDATA Thread_CheckPID (ULONG pid) {
	LPPTDATA data = t_chainstart;
	if (!data) { return nullptr; }
	
	do {
		if (data->pid == pid) { return data; }
	} while ((data = data->next) != nullptr);
	
	return nullptr;
}

static LPPTDATA Thread_GetEmptyThread () {
	LPPTDATA data = t_chainstart;
	if (!data) { return nullptr; }
	
	do {
		if (data->pid == 0) { return data; }
	} while ((data = data->next) != nullptr);
	
	return nullptr;
}

static void Thread_WriteResult (LPPTDATA data) {
	char output[MAX_PATH];
	DWORD cnt;
	DWORD acctime = 0;
	FILETIME start, end, ft[2];
	SYSTEMTIME convert;
	
	GetProcessTimes(data->process, &start, &end, &ft[0], &ft[1]); //Data - Runtime
	
	sprintf_s(output, ARRAYSIZE(output), "%s(%s)\r\n", data->name, data->winname);
	WriteFile(t_file, output, (DWORD)strlen(output), &cnt, nullptr); //Write - Path
	
	FileTimeToLocalFileTime(&start, &ft[0]);
	FileTimeToSystemTime(&ft[0], &convert);
	sprintf_s(output, ARRAYSIZE(output), "*Start Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, (DWORD)strlen(output), &cnt, nullptr); //Write - Start Time
	
	FileTimeToLocalFileTime(&end, &ft[1]);
	FileTimeToSystemTime(&ft[1], &convert);
	sprintf_s(output, ARRAYSIZE(output), "*End Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, (DWORD)strlen(output), &cnt, nullptr); //Write - End Time
	
	Util_DateOperate(&end, &start);
	Util_FileTimeToTime(&end, &convert);
	sprintf_s(output, ARRAYSIZE(output), "*Total Time: %d%s %d%s %d%s\r\n", convert.wHour, DLG_THREAD_HOUR, convert.wMinute, DLG_THREAD_MINUTE, convert.wSecond, DLG_THREAD_SECOND);
	WriteFile(t_file, output, (DWORD)strlen(output), &cnt, nullptr); //Write - Total Time
	
	RegGetValue(m_regrec, nullptr, data->path, RRF_RT_REG_DWORD, nullptr, &acctime, &(cnt = sizeof(DWORD)));
	acctime += convert.wSecond;
	acctime += convert.wMinute * 60;
	acctime += convert.wHour * 3600;
	RegSetValueEx(m_regrec, data->path, 0, REG_DWORD, (BYTE*)&acctime, sizeof(DWORD));
	
	sprintf_s(output, ARRAYSIZE(output), "*Accumulated Time: %d%s %d%s %d%s\r\n\r\n", acctime / 3600, DLG_THREAD_HOUR, (acctime % 3600) / 60, DLG_THREAD_MINUTE, (acctime % 3600) % 60, DLG_THREAD_SECOND);
	WriteFile(t_file, output, (DWORD)strlen(output), &cnt, nullptr); //Write - Accumuulated Time
}

static DWORD Thread_Run (LPVOID lpptdata) {
	LPPTDATA data = (LPPTDATA)lpptdata;
	
	while (true) {
		WaitForSingleObject(data->event, INFINITE);
		WaitForSingleObject(data->process, INFINITE);
		Thread_WriteResult(data);
		CloseHandle(data->process);
		data->process = nullptr;
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

	if (!Thread_OutputFileCreate()) {
		CloseHandle(process);
		return;
	}
	if (!t_chainend) {
		CloseHandle(process);
		return;
	}
	
	check = Thread_CheckPID(pid);
	if (check) {
		MessageBox(nullptr, DLG_THREAD_TIME, L"Running...", MB_OK);
		CloseHandle(process);
		return;
	}
	
	check = Thread_GetEmptyThread();
	if (!check) {
		temp = (LPPTDATA)malloc(sizeof(PTDATA));
		if (!temp) {
			CloseHandle(process);
			return;
		}
		ZeroMemory(temp, sizeof(PTDATA));
		temp->event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		temp->thread = CreateThread(nullptr, 0, Thread_Run, temp, 0, nullptr);
		if (!temp->event || !temp->thread) {
			if (temp->thread) { CloseHandle(temp->thread); }
			if (temp->event) { CloseHandle(temp->event); }
			CloseHandle(process);
			free(temp);
			Util_PrintWindowsLastError();
			return;
		}
		temp->process = process;
		temp->pid = pid;
		cnt = ARRAYSIZE(temp->path);
		QueryFullProcessImageName(process, 0, temp->path, &cnt);
		GetWindowText(hwnd, path, 260);
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, temp->winname, 260, nullptr, nullptr));
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, name, -1, temp->name, 260, nullptr, nullptr));
		SetEvent(temp->event);
		t_chainend->next = temp;
		t_chainend = temp;
	} else {
		check->process = process;
		check->pid = pid;
		cnt = ARRAYSIZE(check->path);
		QueryFullProcessImageName(process, 0, check->path, &cnt);
		GetWindowText(hwnd, path, 260);
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, check->winname, 260, nullptr, nullptr));
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, name, -1, check->name, 260, nullptr, nullptr));
		SetEvent(check->event);
	}
}

void Thread_CreateThreadProcess (LPCWSTR filepath) {
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	LPPTDATA check;
	LPPTDATA temp;
	
	si.cb = sizeof(si);
	if (!CreateProcess(filepath, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		Util_PrintWindowsLastError();
		return;
	}
	CloseHandle(pi.hThread);
	
	if (!Thread_OutputFileCreate()) {
		CloseHandle(pi.hProcess);
		return;
	}
	if (!t_chainend) {
		CloseHandle(pi.hProcess);
		return;
	}
	
	check = Thread_CheckPID(pi.dwProcessId);
	if (check) {
		MessageBox(nullptr, DLG_THREAD_TIME, L"Running...", MB_OK);
		CloseHandle(pi.hProcess);
		return;
	}
	
	check = Thread_GetEmptyThread();
	if (!check) {
		temp = (LPPTDATA)malloc(sizeof(PTDATA));
		if (!temp) {
			CloseHandle(pi.hProcess);
			return;
		}
		ZeroMemory(temp, sizeof(PTDATA));
		temp->event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		temp->thread = CreateThread(nullptr, 0, Thread_Run, temp, 0, nullptr);
		if (!temp->event || !temp->thread) {
			if (temp->thread) { CloseHandle(temp->thread); }
			if (temp->event) { CloseHandle(temp->event); }
			CloseHandle(pi.hProcess);
			free(temp);
			Util_PrintWindowsLastError();
			return;
		}
		temp->process = pi.hProcess;
		temp->pid = pi.dwProcessId;
		wcscpy_s(temp->path, ARRAYSIZE(temp->path), filepath);
		strcpy_s(temp->winname, ARRAYSIZE(temp->winname), "Direct measurement");
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, filepath, -1, temp->name, 260, nullptr, nullptr));
		SetEvent(temp->event);
		t_chainend->next = temp;
		t_chainend = temp;
	} else {
		check->process = pi.hProcess;
		check->pid = pi.dwProcessId;
		wcscpy_s(check->path, ARRAYSIZE(check->path), filepath);
		strcpy_s(check->winname, ARRAYSIZE(check->winname), "Direct measurement");
		AssertWin(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, filepath, -1, check->name, 260, nullptr, nullptr));
		SetEvent(check->event);
	}
}

void Thread_Init () {
	t_chainstart = (LPPTDATA)malloc(sizeof(PTDATA));
	if (!t_chainstart) { return; }
	ZeroMemory(t_chainstart, sizeof(PTDATA));
	t_chainstart->event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	t_chainstart->thread = CreateThread(nullptr, 0, Thread_Run, t_chainstart, 0, nullptr);
	if (!t_chainstart->event || !t_chainstart->thread) {
		if (t_chainstart->thread) { CloseHandle(t_chainstart->thread); }
		if (t_chainstart->event) { CloseHandle(t_chainstart->event); }
		free(t_chainstart);
		t_chainstart = nullptr;
		Util_PrintWindowsLastError();
		return;
	}
	t_chainend = t_chainstart;
}

void Thread_Close () {
	LPPTDATA data = t_chainstart;
	if (!data) { return; }
	
	do {
		t_chainstart = data->next;
		if (data->thread) { TerminateThread(data->thread, 0); CloseHandle(data->thread); }
		if (data->process) { CloseHandle(data->process); }
		if (data->event) { CloseHandle(data->event); }
		free(data);
		data = t_chainstart;
	} while (data);
	
	t_chainstart = nullptr;
	t_chainend = nullptr;
	
	if (t_file) { CloseHandle(t_file); }
	t_file = nullptr;
}
