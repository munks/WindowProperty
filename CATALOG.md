# WindowProperty 코드 카탈로그

주 프로그램(`main`), DLL 주입 보조 프로그램(`dllinjector`), 주입 DLL(`dll`)의 함수와 상태를 빠르게 찾기 위한 색인입니다. `static` 함수와 파일 내부 변수는 해당 구현 파일에서만 사용됩니다. 나머지는 헤더를 통해 다른 번역 단위에서 사용할 수 있습니다.

## 실행 흐름

```text
wWinMain
  ├─ 레지스트리 · 컨트롤 · 런타임 감시 초기화
  ├─ CreateWindowEx → MainProc(WM_CREATE)
  │    ├─ 트레이 메뉴 생성 및 아이콘 등록/재시도
  │    └─ Task Scheduler 상태 조회
  └─ 메시지 루프
       ├─ MainProc(WM_COMMAND) → Process_* → DlgFunction_* / Thread_*
       ├─ MainProc(WM_ICONNOTIFY) → Menu_ExecuteNotifyEvent
       └─ TaskbarCreated → 트레이 아이콘 재등록
```

## 함수

### `main/main.cpp` — 앱 수명 주기와 주 창

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `wWinMain` | 진입점 | 명령줄을 처리하고 전역 리소스, 컨트롤, 주 창을 초기화한 뒤 메시지 루프와 종료 정리를 수행합니다. |
| `MainProc` | 파일 내부 | 주 창 프로시저입니다. 버튼, 트레이, 목록, 복사 데이터, 단축키 메시지를 라우팅합니다. |
| `Main_DeleteRegistryVer0`, `Main_DeleteRegistryVer1`, `Main_VersionCheck` | 파일 내부 | 이전 버전의 레지스트리/시작 항목을 정리하고 저장된 설정 버전을 갱신합니다. |
| `Main_IsWindowResponsive` | 파일 내부 | `SendMessageTimeout(WM_NULL)`로 대상 창이 응답 가능한지 확인합니다. |
| `Main_StartTrayIconRegistration` | 파일 내부 | 트레이 아이콘을 등록하고 실패 시 재시도 타이머를 시작합니다. |
| `Main_RetryTrayIconRegistration` | 파일 내부 | 최대 30회까지 트레이 아이콘 등록을 재시도합니다. |

### `main/menu.cpp` — 트레이 메뉴·시작 작업

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Menu_MakeMenu` | 공개 | 트레이 팝업 메뉴를 만듭니다. |
| `Menu_ExecuteNotifyEvent` | 공개 | 트레이 메뉴 명령(로그 표시, 종료, 시작 등록 등)을 처리합니다. |
| `Menu_AddNotifyIcon`, `Menu_RemoveNotifyIcon` | 공개 | 알림 영역 아이콘을 등록/제거합니다. 등록 후 v4 프로토콜을 설정합니다. |
| `Menu_InfoNotifyIcon` | 공개 | 풍선 알림을 표시하고 로그에도 기록합니다. |
| `Menu_SetMenuState`, `Menu_SetMenuString` | 공개 | 트레이 메뉴 항목의 체크 상태 또는 표시 문자열을 갱신합니다. |
| `Menu_TaskSchedulerInit` | 공개 | Task Scheduler에 연결해 시작 작업 등록 여부를 메뉴에 반영합니다. |
| `Menu_SetStartup` | 파일 내부 | `WindowProperty` 로그온 작업을 등록하거나 제거합니다. |
| `Menu_SetRTContext` | 파일 내부 | `.exe` 탐색기 컨텍스트 메뉴의 런타임 체크 명령을 등록/제거합니다. |
| `Menu_DeleteNotifyIcon` | 파일 내부 | 풍선 알림 내용을 타이머 후 지웁니다. |

### `main/process.cpp` — 선택한 창/프로세스 작업

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Process_WindowPropChange` | 공개 | 창 스타일·확장 스타일 편집 대화상자를 엽니다. |
| `Process_WindowCaptionChange` | 공개 | 창 제목 편집 대화상자를 엽니다. |
| `Process_WindowOpacityChange` | 공개 | 레이어드 창 알파 값을 설정합니다. |
| `Process_WindowFullScreenChange` | 공개 | 대상 창을 모니터 경계에 맞춘 전체 화면 스타일로 전환합니다. |
| `Process_WindowsDLLHook` | 공개 | 대상 프로세스 비트 수를 확인하고 DLL injector를 관리자 권한으로 실행합니다. |
| `Process_OpenDirectory` | 공개 | 대상 프로세스 실행 파일이 있는 폴더를 엽니다. |
| `Process_EnumModule` | 공개 | 대상 프로세스의 모듈 목록을 별도 목록 창에 표시합니다. |
| `Process_RuntimeChecker` | 공개 | 선택 프로세스를 런타임 감시 목록에 추가합니다. |
| `Process_ChangeHotkey`, `Process_ChangeFilter` | 공개 | 각각 단축키와 목록 필터 대화상자를 표시합니다. |
| `Process_WindowMainThread` | 공개 | 대상 창의 UI 스레드를 일시 정지하거나 재개합니다. |
| `HotkeyProc`, `PropProc`, `NameProc` | 파일 내부 | 단축키, 창 속성, 창 제목 대화상자 프로시저입니다. |
| `CheckAbsolutePath` | 파일 내부 | 주 실행 파일과 같은 폴더의 보조 파일 존재 여부를 확인합니다. |
| `ExecuteFromAbsolutePath` | 파일 내부 | 보조 EXE와 DLL 경로를 구성해 `ShellExecuteEx`로 실행합니다. |
| `FormatSEError` | 파일 내부 | ShellExecute 오류 값을 표시 문자열로 변환합니다. |

### `main/control.cpp`, `main/list.cpp`, `main/log.cpp` — UI 구성 요소

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Control_InitDLL` | 공개 | comctl32의 이미지 목록 관련 함수를 동적으로 가져옵니다. |
| `Control_CreateButton`, `Control_CreateEdit`, `Control_CreateStatic`, `Control_CreateListView` | 공개 | 주 창 컨트롤을 만들고 필요한 초기 속성을 설정합니다. |
| `Control_RefreshListView` | 공개 | 현재 최상위 창을 다시 열거해 목록을 갱신합니다. |
| `Control_CreateTooltip` | 파일 내부 | 특정 컨트롤의 툴팁을 생성합니다. |
| `List_CreateWindow` | 공개 | 텍스트 결과를 보여 주는 목록 창을 생성합니다. |
| `List_ShowWindow`, `List_AddText`, `List_RemoveText` | 공개 | 목록 창을 표시하고 텍스트를 추가/초기화합니다. |
| `ListProc` | 파일 내부 | 목록 창의 창 프로시저입니다. |
| `Log_CreateWindow` | 공개 | 로그 팝업 창과 읽기 전용 편집 컨트롤을 생성합니다. |
| `Log_Message` | 공개 | 타임스탬프가 붙은 메시지를 로그 편집 컨트롤에 추가합니다. |
| `AddTextToLogEdit`, `LogProc` | 파일 내부 | 로그 텍스트 삽입 및 로그 창 메시지를 처리합니다. |

### `main/dlgfunction.cpp` — 공통 대화상자 로직

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `DlgFunction_ResetFilter` | 공개 | 필터 편집 컨트롤을 기본값으로 되돌립니다. |
| `DlgFunction_SetProperties`, `DlgFunction_SetFilter` | 공개 | 대화상자 입력값을 창 속성 또는 필터 전역 상태에 반영합니다. |
| `DlgFunction_PropInit` | 공개 | 속성/필터 대화상자의 체크 상태와 설명을 초기화합니다. |
| `DlgFunction_PropButtonState` | 공개 | 속성 비트에 따른 버튼 체크 상태를 바꿉니다. |
| `DlgFunction_SystemTimeLoop` | 공개 | 속성 대화상자의 경과 시간을 주기적으로 갱신합니다. |
| `DlgFunction_HotkeyValidCheck` | 공개 | 서로 다른 두 단축키가 유효하게 지정됐는지 확인합니다. |
| `DlgFunction_CheckButtonState` | 파일 내부 | 가변 인수로 받은 체크 버튼들의 비트 상태를 합칩니다. |
| `DlgFunction_ButtonSetType` | 파일 내부 | 버튼에 대응하는 창 스타일/확장 스타일 종류를 연결합니다. |

### `main/hook.cpp` — 단축키와 마우스 제어

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Hook_MoveHotkeyRegister` | 공개 | 이동 활성화 단축키를 등록하거나 해제합니다. |
| `Hook_ClipHotkeyRegister` | 공개 | 커서 가두기 단축키를 등록하거나 해제합니다. |
| `Hook_MouseHook` | 공개 | 다음 드래그 대상 창을 감지하기 위한 마우스 훅을 설치합니다. |
| `Hook_ClipCursor` | 공개 | 활성 창 경계로 커서를 제한하거나 제한을 해제합니다. |
| `MoveHook` | 파일 내부 | 마우스 드래그로 대상 창을 이동시키는 훅 콜백입니다. |
| `ClipCheck` | 파일 내부 | 커서 제한 대상 창의 상태를 감시하는 스레드 함수입니다. |

### `main/thread.cpp` — 런타임 감시

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Thread_Init`, `Thread_Close` | 공개 | 런타임 감시 체인을 만들고 종료 시 모든 감시 스레드/핸들을 정리합니다. |
| `Thread_CreateThread` | 공개 | 창 핸들에서 PID를 구해 해당 프로세스 감시를 시작합니다. |
| `Thread_CreateThreadProcess` | 공개 | 파일을 직접 실행한 뒤 생성된 프로세스를 감시합니다. |
| `Thread_OutputFileCreate` | 파일 내부 | `record` 폴더와 실행 시간 기록 파일을 준비합니다. |
| `Thread_CheckPID`, `Thread_GetEmptyThread` | 파일 내부 | 중복 PID 또는 재사용 가능한 감시 노드를 찾습니다. |
| `Thread_WriteResult` | 파일 내부 | 종료 시간, 실행 시간, 누적 시간을 파일 및 레지스트리에 기록합니다. |
| `Thread_Run` | 파일 내부 | 종료 이벤트와 프로세스 핸들을 기다리는 감시 스레드 본문입니다. |

### `main/util.cpp`, `main/vk.cpp`, `main/debug.cpp` — 보조 기능

| 함수 | 공개 범위 | 역할 |
| --- | --- | --- |
| `Util_GetProcessID` | 공개 | 창 핸들에 연결된 프로세스 ID를 반환합니다. |
| `Util_WindowFilter` | 공개 | 전역 스타일 필터로 창을 목록에 포함할지 판정합니다. |
| `Util_GetHotkey`, `Util_SetHotkey`, `Util_GetHotkeyString` | 공개 | 단축키 레지스트리 값을 읽고 쓰거나 표시 문자열로 만듭니다. |
| `Util_PrintWindowsLastErrorInternal`, `Util_PrintInt`, `Util_PrintString` | 공개 | 오류 코드 또는 진단 값을 메시지 상자로 표시합니다. |
| `Util_GetWDAState` | 공개 | 창의 캡처 제외(Display Affinity) 상태를 반환합니다. |
| `Util_DateOperate`, `Util_FileTimeToTime` | 공개 | FILETIME 차이를 계산하고 사람이 읽을 시간으로 변환합니다. |
| `operator==`, `operator!=` | 공개 | `RECT` 값 비교 연산자입니다. |
| `Util_GetHotkeyRegkey` | 파일 내부 | 단축키 종류에 대응하는 레지스트리 값 이름을 반환합니다. |
| `VirtualKeyCodeText` | 공개 | 가상 키 코드를 사람이 읽을 키 이름으로 변환합니다. |
| `Debug_PrintWindowStyle`, `Debug_PrintWindowExStyle`, `Debug_ConvertWindowMessage` | Debug 전용 | 창 스타일 및 Win32 메시지를 콘솔에 진단 출력합니다. |
| `PrintOutputTextSixTab` | 파일 내부 | Debug 출력 형식을 맞추는 보조 함수입니다. |

### `dllinjector/main.cpp` — 원격 DLL 주입 도우미

| 함수 | 역할 |
| --- | --- |
| `wmain` | PID와 DLL 경로 인수를 검증한 뒤 권한을 설정하고 주입을 시작합니다. |
| `SetTokenPrivileges` | 현재 프로세스에 `SeDebugPrivilege` 권한을 요청합니다. |
| `ProcessCommandLine` | 대상 프로세스와 DLL 파일을 검증한 뒤 주입을 호출합니다. |
| `AttachDLL` | 원격 메모리에 DLL 경로를 기록하고 `LoadLibraryW` 원격 스레드를 실행합니다. |
| `ErrorMsg` | Win32 오류 코드를 포함한 오류 상자를 표시합니다. |

### `dll/dllmain.cpp` — 주입 DLL

| 함수 | 역할 |
| --- | --- |
| `DllMain` | 프로세스 attach 시 후속 작업 스레드를 시작합니다. |
| `TypeCheck` | 레지스트리의 `LastHook` 유형에 따라 명령줄 표시 또는 캡처 상태 변경을 선택합니다. |
| `CommandLine` | 대상 프로세스 명령줄을 클립보드와 메시지 상자에 표시합니다. |
| `CaptureAffinity` | 레지스트리의 대상 창에 캡처 포함/제외 상태를 적용합니다. |

## 전역 상태

| 정의 파일 | 변수 | 의미 |
| --- | --- | --- |
| `main.cpp` | `m_main`, `m_hInstance`, `m_hbrush`, `m_font` | 주 창 및 공용 UI 리소스입니다. |
| `main.cpp` | `m_regkey`, `m_regset`, `m_regrec` | 기본 설정, 하위 설정, 런타임 기록 레지스트리 키입니다. |
| `main.cpp` | `WM_TASKBARCREATED` | Explorer 재시작을 알리는 등록 메시지 ID입니다. |
| `main.cpp` | `m_trayRetryAttempts` | 현재 트레이 아이콘 등록 재시도 횟수입니다. |
| `menu.cpp` | `me_menu`, `me_mi` | 트레이 메뉴와 메뉴 항목 작업 버퍼입니다. |
| `menu.cpp` | `me_nid`, `me_timer` | 알림 영역 아이콘 데이터와 풍선 알림 정리 타이머입니다. |
| `control.cpp` | `c_comctlModule`, `c_imageList` | comctl32 모듈 및 창 목록 아이콘 목록입니다. |
| `control.cpp` | `c_listView`, `c_listViewIndex` | 주 창의 창 목록 컨트롤과 선택 인덱스입니다. |
| `list.cpp` | `li_window`, `li_edit` | 모듈 등 텍스트 결과 창과 편집 컨트롤입니다. |
| `log.cpp` | `l_window`, `l_logmsg` | 로그 창과 로그 편집 컨트롤입니다. |
| `process.cpp` | `p_tdata`, `p_dlgType` | 속성 대화상자 대상/타이머 상태 및 현재 대화상자 종류입니다. |
| `thread.cpp` | `t_chainstart`, `t_chainend` | 런타임 감시 노드 연결 리스트의 시작과 끝입니다. |
| `thread.cpp` | `t_createfirst`, `t_file` | 기록 파일 최초 생성 여부와 열린 파일 핸들입니다. |
| `hook.cpp` | `h_hookMove`, `h_hookClip` | 이동용 마우스 훅과 커서 제한 감시 스레드 핸들입니다. |
| `hook.cpp` | `h_handlePoint`, `h_mousePoint`, `h_moveHandle` | 창 드래그를 위한 좌표와 현재 대상 창입니다. |
| `util.cpp` | `u_filter[2][2]` | 목록에 포함/제외할 일반·확장 창 스타일 필터입니다. |
| `debug.cpp` | `d_count` | Debug 출력의 줄/탭 상태입니다. |

## 핵심 자료형

| 자료형 | 위치 | 용도 |
| --- | --- | --- |
| `PTDATA` | `include/thread.hpp` | 감시 대상 프로세스의 PID, 경로, 이벤트, 스레드, 연결 리스트 포인터를 보관합니다. |
| `TDATA` | `include/dlgfunction.hpp` | 속성 대화상자의 대상 창, 타이머 창, 시작 FILETIME을 보관합니다. |
| `LVINFO` | `include/control.hpp` | 창 목록 열의 제목과 폭 비율을 보관합니다. |
