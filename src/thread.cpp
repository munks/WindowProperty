#include "global.hpp"

LPPTDATA t_chainstart;
LPPTDATA t_chainend;
bool t_createfirst = true;
HANDLE t_file;

//Internal

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

void Thread_WriteResult (HANDLE process) {
	char path[MAX_PATH];
	char output[MAX_PATH];
	DWORD cnt;
	FILETIME start, end, ft[2];
	SYSTEMTIME convert;
	
	if (!GetProcessImageFileNameA(process, path, MAX_PATH)) { Util_PrintWindowsLastError(); } //Data - Path
	GetProcessTimes(process, &start, &end, &ft[0], &ft[1]); //Data - Runtime
	
	sprintf(output, "*%s\r\n", strrchr(path, '\\') + 1);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Path
	
	FileTimeToLocalFileTime(&start, &ft[0]);
	FileTimeToSystemTime(&ft[0], &convert);
	sprintf(output, "Start Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Start Time
	
	FileTimeToLocalFileTime(&end, &ft[1]);
	FileTimeToSystemTime(&ft[1], &convert);
	sprintf(output, "End Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", convert.wYear, convert.wMonth, convert.wDay, convert.wHour, convert.wMinute, convert.wSecond);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - End Time
	
	Util_DateOperate(&end, &start);
	Util_FileTimeToTime(&end, &convert);
	sprintf(output, "Total Time: %d%s %d%s %d%s\r\n\r\n", convert.wHour, DLG_THREAD_HOUR, convert.wMinute, DLG_THREAD_MINUTE, convert.wSecond, DLG_THREAD_SECOND);
	WriteFile(t_file, output, strlen(output), &cnt, NULL); //Write - Total Time
}

DWORD Thread_Run (LPVOID lpptdata) {
	LPPTDATA data = (LPPTDATA)lpptdata;
	
	while (true) {
		WaitForSingleObject(data->event, INFINITE);
		WaitForSingleObject(data->process, INFINITE);
		Thread_WriteResult(data->process);
		CloseHandle(data->process);
		data->process = NULL;
		data->pid = 0;
	}
	
	return 0;
}

//External
void Thread_CreateThread (ULONG pid) {
	LPPTDATA check;
	HANDLE process;
	LPPTDATA temp;
	wchar_t path[MAX_PATH];
	wchar_t file[MAX_PATH];
	SYSTEMTIME time;
	
	process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if (!process) {
		Util_PrintWindowsLastError();
		return;
	}
	
	if (t_createfirst) {
		t_createfirst = false;
		
		GetCurrentDirectory(MAX_PATH, path);
		GetLocalTime(&time);
		swprintf(file, L"%s\\ProcessTimeRecord %04d-%02d-%02d %02d-%02d-%02d.txt", path, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		t_file = CreateFile(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!t_file) { Util_PrintWindowsLastError(); }
	}
	
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
		SetEvent(temp->event);
		t_chainend->next = temp;
		t_chainend = temp;
	} else {
		check->process = process;
		check->pid = pid;
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