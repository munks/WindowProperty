#include <stdio.h>
#include <wchar.h>
#include <windows.h>

void AttachDLL (HANDLE process, const wchar_t* dll, int dlllen) {
	LPVOID alloc;
	HMODULE gmh;
	HANDLE thread;
	
	//Allocate/Write Memory
	alloc = VirtualAllocEx(process, NULL, sizeof(wchar_t) * dlllen, MEM_COMMIT, PAGE_READWRITE);
	if (!alloc) {
		puts("VirtualAllocEx Error");
		return;
	}
	
	WriteProcessMemory(process, alloc, (void*)dll, sizeof(wchar_t) * dlllen, NULL);
	
	//Run Thread
	thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, alloc, 0, NULL);
	if (!thread) {
		puts("CreateRemoteThread Error");
		goto THREADCLOSE;
	}
	
	WaitForSingleObject(thread, INFINITE);
	
	CloseHandle(thread);
	THREADCLOSE:
	VirtualFreeEx(process, alloc, sizeof(wchar_t) * dlllen, MEM_DECOMMIT);
	return;
}

void ProcessCommandLine (ULONG pid, const wchar_t* dll) {
	HANDLE process;
	wchar_t path[260];
	int pathlen;
	FILE* file;
	
	//OpenProcess
	process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, pid);
	if (!process) {
		puts("OpenProcess Error");
		return;
	}
	
	//DLL Check
	GetModuleFileName(NULL, path, 260);
	*(wcsrchr(path, L'\\') + 1) = L'\0';
	wcscat(path, dll);
	pathlen = wcslen(path) + 1;
	
	if (pathlen > 260) {
		goto FREEPROC;
	}
	
	file = _wfopen(path, L"r");
	if (!file) {
		goto FREEPROC;
	}
	fclose(file);
	
	AttachDLL(process, path, pathlen);
	
	FREEPROC:
	CloseHandle(process);
}

int wmain (int argc, wchar_t* argv[]) {
	if (argc != 3) { return 0; }
	
	ProcessCommandLine(_wtoi(argv[1]), argv[2]);
	return 0;
}