#pragma once

typedef struct PTDATA PTDATA, *LPPTDATA;

typedef struct PTDATA {
	ULONG pid;
	HANDLE thread;
	HANDLE process;
	HANDLE event;
	LPPTDATA next;
} PTDATA, *LPPTDATA;

void Thread_CreateThread (ULONG);
void Thread_Init ();
void Thread_Close ();