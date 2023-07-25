#include <stdio.h>
#include <wchar.h>
#include <windows.h>

void AttachDLL (HANDLE process, const wchar_t* dll) {
	LPVOID alloc;
	HMODULE gmh;
	HANDLE thread;
	int len;
	
	len = wcslen(dll) + 1;
	
	//Allocate/Write Memory
	alloc = VirtualAllocEx(process, NULL, sizeof(wchar_t) * len, MEM_COMMIT, PAGE_READWRITE);
	if (!alloc) {
		puts("VirtualAllocEx Error");
		return;
	}
	
	WriteProcessMemory(process, alloc, (void*)dll, sizeof(wchar_t) * len, NULL);
	
	//Run Thread
	thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, alloc, 0, NULL);
	if (!thread) {
		puts("CreateRemoteThread Error");
		goto THREADCLOSE;
	}
	
	WaitForSingleObject(thread, INFINITE);
	
	CloseHandle(thread);
	THREADCLOSE:
	VirtualFreeEx(process, alloc, sizeof(wchar_t) * len, MEM_DECOMMIT);
	return;
}

void ProcessCommandLine (ULONG pid, const wchar_t* dll) {
	HANDLE process;
	FILE* file;
	
	//OpenProcess
	process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, pid);
	if (!process) {
		puts("OpenProcess Error");
		return;
	}
	
	//DLL Check
	file = _wfopen(dll, L"r");
	if (!file) {
		goto FREEPROC;
	}
	fclose(file);
	
	//Attach DLL
	AttachDLL(process, dll);
	
	FREEPROC:
	CloseHandle(process);
}

int wmain (int argc, wchar_t* argv[]) {
	if (argc != 3) { return 0; }
	
	ProcessCommandLine(_wtoi(argv[1]), argv[2]);
	return 0;
}