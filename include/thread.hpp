#pragma once

typedef struct PTDATA PTDATA, *LPPTDATA;

typedef struct PTDATA {
	char name[260];
	char winname[260];
	wchar_t path[260];
	ULONG pid;
	HANDLE thread;
	HANDLE process;
	HANDLE event;
	LPPTDATA next;
} PTDATA, *LPPTDATA;

void Thread_CreateThread (HWND, LPCWSTR);
void Thread_CreateThreadProcess (LPCWSTR);
void Thread_Init ();
void Thread_Close ();