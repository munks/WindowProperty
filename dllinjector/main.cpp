#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <format>

void ErrorMsg (LPCWSTR msg) {
	MessageBox(nullptr, std::format(L"{} Error\n-Error: {:x}", msg, GetLastError()).c_str(), L"Error", MB_OK | MB_ICONERROR);
}
void AttachDLL (HANDLE process, const wchar_t* dll) {
	LPVOID alloc;
	HANDLE thread;
	size_t len;
	
	len = wcslen(dll) + 1;
	
	//Allocate/Write Memory
	alloc = VirtualAllocEx(process, nullptr, sizeof(wchar_t) * len, MEM_COMMIT, PAGE_READWRITE);
	if (!alloc) {
		ErrorMsg(L"VirtualAllocEx");
		return;
	}
	
	if (!WriteProcessMemory(process, alloc, dll, sizeof(wchar_t) * len, nullptr)) {
		ErrorMsg(L"WriteProcessMemory");
		goto THREADCLOSE;
	}
	
	//Run Thread
	thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, alloc, 0, nullptr);
	if (!thread) {
		ErrorMsg(L"CreateRemoteThread");
		goto THREADCLOSE;
	}
	
	WaitForSingleObject(thread, INFINITE);
	
	CloseHandle(thread);
	THREADCLOSE:
	VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
	return;
}

void ProcessCommandLine (ULONG pid, const wchar_t* dll) {
	HANDLE process;
	FILE* file;
	
	//OpenProcess
	process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, false, pid);
	if (!process) {
		ErrorMsg(L"OpenProcess");
		return;
	}
	
	//DLL Check
	if (_wfopen_s(&file, dll, L"r") != 0) {
		ErrorMsg(L"Failed to open DLL file");
		goto FREEPROC;
	}
	fclose(file);
	
	//Attach DLL
	AttachDLL(process, dll);
	
	FREEPROC:
	CloseHandle(process);
}

void SetTokenPrivileges() {
	HANDLE token = nullptr;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) { return; }
	if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
		CloseHandle(token);
		return;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr);
	CloseHandle(token);
}

int wmain (int argc, wchar_t* argv[]) {
	if (argc != 3) { return 0; }
	
	SetTokenPrivileges();

	ProcessCommandLine(_wtoi(argv[1]), argv[2]);
	return 0;
}
