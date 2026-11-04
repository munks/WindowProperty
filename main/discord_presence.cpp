#include "discord_presence.hpp"
#include "global.hpp"
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <mutex>

namespace {
constexpr char ApplicationId[] = "1567673678072914011";
constexpr DWORD MaxFrame = 64 * 1024;
constexpr ULONGLONG TimerRefreshMs = 10000;
HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
HANDLE updateEvent = nullptr;
std::mutex settingsMutex;
std::string activityDetails;
std::string activityName;
long long programStarted = 0;
long long fixedSeconds = -1;

std::string JsonEscape(const std::string& text) {
    static constexpr char hex[] = "0123456789abcdef";
    std::string result;
    for (unsigned char ch : text) {
        if (ch == '"' || ch == '\\') { result += '\\'; result += static_cast<char>(ch); }
        else if (ch < 0x20) { result += "\\u00"; result += hex[ch >> 4]; result += hex[ch & 15]; }
        else result += static_cast<char>(ch);
    }
    return result;
}

bool ToUtf8(const wchar_t* text, std::string& result) {
    int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1, nullptr, 0, nullptr, nullptr);
    if (!size || size - 1 > 128) return false;
    result.resize(size);
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1, result.data(), size, nullptr, nullptr)) return false;
    result.pop_back();
    return true;
}

struct Header { uint32_t opcode; uint32_t length; };

bool WriteFrame(HANDLE pipe, uint32_t opcode, const std::string& body) {
    Header header{opcode, static_cast<uint32_t>(body.size())};
    DWORD written = 0;
    return WriteFile(pipe, &header, sizeof(header), &written, nullptr) && written == sizeof(header)
        && WriteFile(pipe, body.data(), header.length, &written, nullptr) && written == header.length;
}

// Poll first so shutdown and a missing or partial response never block the UI.
bool ReadBytes(HANDLE pipe, void* target, DWORD length) {
    auto* bytes = static_cast<char*>(target);
    DWORD total = 0;
    while (total < length) {
        if (WaitForSingleObject(stopEvent, 50) == WAIT_OBJECT_0) return false;
        DWORD available = 0;
        if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &available, nullptr)) return false;
        if (!available) continue;
        DWORD read = 0;
        if (!ReadFile(pipe, bytes + total, min(available, length - total), &read, nullptr) || !read) return false;
        total += read;
    }
    return true;
}

bool ReadFrame(HANDLE pipe, Header& header, std::string& body) {
    if (!ReadBytes(pipe, &header, sizeof(header)) || header.length > MaxFrame) return false;
    body.resize(header.length);
    return header.length == 0 || ReadBytes(pipe, body.data(), header.length);
}

HANDLE OpenDiscord() {
    for (int i = 0; i < 10; ++i) {
        wchar_t name[64];
        swprintf_s(name, L"\\\\.\\pipe\\discord-ipc-%d", i);
        HANDLE pipe = CreateFileW(name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (pipe != INVALID_HANDLE_VALUE) return pipe;
    }
    return INVALID_HANDLE_VALUE;
}

bool SendActivity(HANDLE pipe) {
    std::lock_guard<std::mutex> guard(settingsMutex);
    const long long started = fixedSeconds < 0
        ? programStarted : static_cast<long long>(time(nullptr)) - fixedSeconds;
    std::string timeField = "\"timestamps\":{\"start\":" + std::to_string(started) + "}";
    static unsigned long long nonce = 0;
    std::string activity = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + std::to_string(GetCurrentProcessId())
        + ",\"activity\":{" + (activityName.empty() ? "" : "\"name\":\"" + JsonEscape(activityName) + "\",")
        + (activityDetails.empty() ? "" : "\"details\":\"" + JsonEscape(activityDetails) + "\",")
        + timeField + "}},\"nonce\":\"" + std::to_string(++nonce) + "\"}";
    return WriteFrame(pipe, 1, activity);
}

DWORD WINAPI Run(void*) {
    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
        HANDLE pipe = OpenDiscord();
        if (pipe != INVALID_HANDLE_VALUE) {
            std::string hello = std::string("{\"v\":1,\"client_id\":\"") + ApplicationId + "\"}";
            Header header{};
            std::string body;
            if (WriteFrame(pipe, 0, hello) && ReadFrame(pipe, header, body)
                && header.opcode == 1 && body.find("\"READY\"") != std::string::npos) {
                ResetEvent(updateEvent);
                if (SendActivity(pipe)) {
                    ULONGLONG lastUpdate = GetTickCount64();
                    while (WaitForSingleObject(stopEvent, 50) != WAIT_OBJECT_0) {
                        const bool changed = WaitForSingleObject(updateEvent, 0) == WAIT_OBJECT_0;
                        bool fixedTimer = false;
                        {
                            std::lock_guard<std::mutex> guard(settingsMutex);
                            fixedTimer = fixedSeconds >= 0;
                        }
                        if (changed || (fixedTimer && GetTickCount64() - lastUpdate >= TimerRefreshMs)) {
                            if (!SendActivity(pipe)) break;
                            lastUpdate = GetTickCount64();
                        }
                        DWORD available = 0;
                        if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &available, nullptr)) break;
                        if (!available) continue;
                        if (!ReadFrame(pipe, header, body)) break;
                        if (header.opcode == 2) break;
                        if (header.opcode == 3 && !WriteFrame(pipe, 4, body)) break;
                    }
                }
            }
            CloseHandle(pipe);
        }
        if (WaitForSingleObject(stopEvent, 5000) == WAIT_OBJECT_0) break;
    }
    return 0;
}
}

namespace {
void ReadSettings(bool& enabled, wchar_t (&details)[129], wchar_t (&name)[129], wchar_t (&seconds)[10]) {
    DWORD value = 0, size = sizeof(value);
    enabled = RegGetValueW(m_regset, nullptr, L"DiscordEnabled", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS && value == 1;
    size = sizeof(details);
    if (RegGetValueW(m_regset, nullptr, L"DiscordDetails", RRF_RT_REG_SZ, nullptr, details, &size) != ERROR_SUCCESS)
        wcscpy_s(details, DISCORD_DEFAULT_DETAILS);
    size = sizeof(name);
    if (RegGetValueW(m_regset, nullptr, L"DiscordName", RRF_RT_REG_SZ, nullptr, name, &size) != ERROR_SUCCESS) name[0] = L'\0';
    size = sizeof(seconds);
    if (RegGetValueW(m_regset, nullptr, L"DiscordFixedSeconds", RRF_RT_REG_SZ, nullptr, seconds, &size) != ERROR_SUCCESS) seconds[0] = L'\0';
}

bool ParseSeconds(const wchar_t* value, long long& result) {
    if (!*value) { result = -1; return true; }
    result = 0;
    for (; *value; ++value) {
        if (*value < L'0' || *value > L'9') return false;
        result = result * 10 + (*value - L'0');
        if (result > 315360000) return false;
    }
    return true;
}

INT_PTR CALLBACK SettingsProc(HWND dialog, UINT message, WPARAM wParam, LPARAM) {
    if (message == WM_INITDIALOG) {
        bool enabled = false;
        wchar_t details[129]{};
        wchar_t name[129]{};
        wchar_t seconds[10]{};
        ReadSettings(enabled, details, name, seconds);
        SetWindowTextW(dialog, DISCORD_TITLE);
        SetDlgItemTextW(dialog, ID_DISCORD_ENABLED, DISCORD_ENABLED_TEXT);
        SetDlgItemTextW(dialog, ID_DISCORD_NAME_LABEL, DISCORD_NAME_LABEL);
        SetDlgItemTextW(dialog, ID_DISCORD_NAME, name);
        SendDlgItemMessageW(dialog, ID_DISCORD_NAME, EM_SETLIMITTEXT, 128, 0);
        SetDlgItemTextW(dialog, ID_DISCORD_LABEL, DISCORD_DETAILS_LABEL);
        SetDlgItemTextW(dialog, ID_DISCORD_SECONDS_LABEL, DISCORD_SECONDS_LABEL);
        SetDlgItemTextW(dialog, ID_DISCORD_SECONDS, seconds);
        SendDlgItemMessageW(dialog, ID_DISCORD_SECONDS, EM_SETLIMITTEXT, 9, 0);
        SetDlgItemTextW(dialog, ID_DISCORD_DETAILS, details);
        SendDlgItemMessageW(dialog, ID_DISCORD_DETAILS, EM_SETLIMITTEXT, 128, 0);
        CheckDlgButton(dialog, ID_DISCORD_ENABLED, enabled ? BST_CHECKED : BST_UNCHECKED);
        SetDlgItemTextW(dialog, IDOK, DISCORD_CONFIRM);
        SetDlgItemTextW(dialog, IDCANCEL, DISCORD_CANCEL);
        return TRUE;
    }
    if (message == WM_COMMAND && LOWORD(wParam) == IDCANCEL) { EndDialog(dialog, 0); return TRUE; }
    if (message == WM_CLOSE) { EndDialog(dialog, 0); return TRUE; }
    if (message == WM_COMMAND && LOWORD(wParam) == IDOK) {
        wchar_t details[129]{};
        wchar_t name[129]{};
        wchar_t seconds[10]{};
        GetDlgItemTextW(dialog, ID_DISCORD_DETAILS, details, ARRAYSIZE(details));
        GetDlgItemTextW(dialog, ID_DISCORD_NAME, name, ARRAYSIZE(name));
        GetDlgItemTextW(dialog, ID_DISCORD_SECONDS, seconds, ARRAYSIZE(seconds));
        long long parsedSeconds = -1;
        if (!ParseSeconds(seconds, parsedSeconds)) {
            MessageBoxW(dialog, DISCORD_SECONDS_ERROR, DISCORD_ERROR_TITLE, MB_OK | MB_ICONWARNING);
            return TRUE;
        }
        std::string utf8, nameUtf8;
        if (!ToUtf8(details, utf8) || !ToUtf8(name, nameUtf8)) {
            MessageBoxW(dialog, DISCORD_LENGTH_ERROR, DISCORD_ERROR_TITLE, MB_OK | MB_ICONWARNING);
            return TRUE;
        }
        DWORD enabled = IsDlgButtonChecked(dialog, ID_DISCORD_ENABLED) == BST_CHECKED ? 1 : 0;
        if (RegSetValueExW(m_regset, L"DiscordFixedSeconds", 0, REG_SZ, reinterpret_cast<const BYTE*>(seconds), static_cast<DWORD>((wcslen(seconds) + 1) * sizeof(wchar_t))) != ERROR_SUCCESS ||
            RegSetValueExW(m_regset, L"DiscordName", 0, REG_SZ, reinterpret_cast<const BYTE*>(name), static_cast<DWORD>((wcslen(name) + 1) * sizeof(wchar_t))) != ERROR_SUCCESS ||
            RegSetValueExW(m_regset, L"DiscordDetails", 0, REG_SZ, reinterpret_cast<const BYTE*>(details), static_cast<DWORD>((wcslen(details) + 1) * sizeof(wchar_t))) != ERROR_SUCCESS ||
            RegSetValueExW(m_regset, L"DiscordEnabled", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&enabled), sizeof(enabled)) != ERROR_SUCCESS) {
            MessageBoxW(dialog, DISCORD_SAVE_ERROR, DISCORD_ERROR_TITLE, MB_OK | MB_ICONERROR);
            return TRUE;
        }
        if (!enabled) DiscordPresence_Stop();
        {
            std::lock_guard<std::mutex> guard(settingsMutex);
            activityDetails = std::move(utf8);
            activityName = std::move(nameUtf8);
            fixedSeconds = parsedSeconds;
        }
        if (enabled) {
            if (worker) SetEvent(updateEvent);
            else DiscordPresence_Start();
        }
        EndDialog(dialog, 1);
        return TRUE;
    }
    return FALSE;
}
}

void DiscordPresence_Initialize() {
    bool enabled = false;
    wchar_t details[129]{};
    wchar_t name[129]{};
    wchar_t seconds[10]{};
    FILETIME created{}, exited{}, kernel{}, user{};
    if (GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
        ULARGE_INTEGER ticks{};
        ticks.LowPart = created.dwLowDateTime;
        ticks.HighPart = created.dwHighDateTime;
        programStarted = static_cast<long long>(ticks.QuadPart / 10000000ULL - 11644473600ULL);
    } else {
        programStarted = static_cast<long long>(time(nullptr));
    }
    ReadSettings(enabled, details, name, seconds);
    if (!ToUtf8(details, activityDetails)) ToUtf8(DISCORD_DEFAULT_DETAILS, activityDetails);
    if (!ToUtf8(name, activityName)) activityName.clear();
    if (!ParseSeconds(seconds, fixedSeconds)) fixedSeconds = -1;
    if (enabled) DiscordPresence_Start();
}

void DiscordPresence_ShowSettings() {
    DialogBoxParamW(m_hInstance, MAKEINTRESOURCEW(ID_DLG_DISCORD), m_main, SettingsProc, 0);
}

void DiscordPresence_Start() {
    if (worker) return;
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return;
    updateEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!updateEvent) { CloseHandle(stopEvent); stopEvent = nullptr; return; }
    worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) { CloseHandle(updateEvent); CloseHandle(stopEvent); updateEvent = nullptr; stopEvent = nullptr; }
}

void DiscordPresence_Stop() {
    if (!worker) return;
    SetEvent(stopEvent);
    CancelSynchronousIo(worker);
    WaitForSingleObject(worker, INFINITE);
    CloseHandle(worker);
    CloseHandle(stopEvent);
    CloseHandle(updateEvent);
    worker = nullptr;
    stopEvent = nullptr;
    updateEvent = nullptr;
}
