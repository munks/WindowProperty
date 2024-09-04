#pragma once

//Window Main
#define WINDOW_MAIN_NAME			L"DualityFrame_Property"
#define WINDOW_MAIN_CAPTION			L"Process"

//Window Log
#define WINDOW_LOG_NAME				L"DualityFrame_Property_Log"
#define WINDOW_LOG_CAPTION			L"Log"

//Window Hotkey
#define WINDOW_HOTKEY_NAME			L"DualityFrame_Property_Hotkey"
#define WINDOW_HOTKEY_CAPTION		L"Hotkey"

//Window List
#define WINDOW_LIST_NAME			L"DualityFrame_Property_List"
#define WINDOW_LIST_CAPTION			L"List"

//ID_BUTTON_PROP
#define BUTTON_PROP_CAPTION			L"속성"
#define BUTTON_PROP_TOOLTIP			L"창의 속성 변경"

//ID_BUTTON_NAME
#define BUTTON_NAME_CAPTION			L"이름 변경"
#define BUTTON_NAME_TOOLTIP			L"창의 제목 표시줄 변경"

//ID_BUTTON_SCREEN
#define BUTTON_SCREEN_CAPTION		L"전체 화면"
#define BUTTON_SCREEN_TOOLTIP		L"창을 전체 화면으로 변경"

//ID_BUTTON_CMD
#define BUTTON_CMD_CAPTION			L"커맨드 라인"
#define BUTTON_CMD_TOOLTIP			L"프로세스의 명령줄 매개변수 확인 (dllinjector 필요)"

//ID_BUTtON_OPACITY
#define BUTTON_OPACITY_CAPTION		L"불투명도 변경"
#define BUTTON_OPACITY_TOOLTIP		L"창의 불투명도 설정"

//ID_BUTTON_MOVE
#define BUTTON_MOVE_CAPTION			L"이동 활성화"
#define BUTTON_MOVE_TOOLTIP			L"창 이동 후킹 활성화/비활성화"

//ID_BUTTON_OPEN
#define BUTTON_OPEN_CAPTION			L"폴더 열기"
#define BUTTON_OPEN_TOOLTIP			L"실행 파일 경로 열기"

//ID_BUTTON_CAPTURE
#define BUTTON_CAPTURE_CAPTION		L"캡쳐 포함"
#define BUTTON_CAPTURE_CAPTION_2	L"캡쳐 제외"
#define BUTTON_CAPTURE_TOOLTIP		L"창을 캡쳐 화면에서 제외/포함 (dllinjector 필요)"

//ID_BUTTON_HOTKEY
#define BUTTON_HOTKEY_CAPTION		L"단축키 변경"
#define BUTTON_HOTKEY_TOOLTIP		L"유틸리티 단축키 변경"

//ID_BUTTON_FILTER
#define BUTTON_FILTER_CAPTION		L"검색 필터 변경"
#define BUTTON_FILTER_TOOLTIP		L"검색할 창의 필터를 변경 (체크: 포함, 불확정 체크: 제외)"

//ID_BUTTON_CLIP
#define BUTTON_CLIP_CAPTION			L"커서 가두기"
#define BUTTON_CLIP_TOOLTIP			L"창의 영역 안으로 커서를 제한"

//ID_BUTTON_MODULE
#define BUTTON_MODULE_CAPTION		L"모듈 확인"
#define BUTTON_MODULE_TOOLTIP		L"프로세스에 로드된 모듈 목록 확인"

//ID_BUTTON_RUNTIME
#define BUTTON_RUNTIME_CAPTION		L"런타임 체크"
#define BUTTON_RUNTIME_TOOLTIP		L"프로세스 종료 시 시작 시간, 종료 시간, 가동 시간을 기록합니다"

//ID_LIST
#define LIST_TOOLTIP				L"프로세스 목록"

//Change
#define CHANGE_TEXT					L"변경"

//Notify
#define NOTIFY_HOTKEY				L"단축키 눌림"
#define NOTIFY_MOVE_ACTIVE			L"창 이동 후킹 활성화"
#define NOTIFY_MOVE_DEACTIVE		L"창 이동 후킹 비활성화"
#define NOTIFY_CLIP_ACTIVE			L"커서 가두기 활성화"
#define NOTIFY_CLIP_DEACTIVE		L"커서 가두기 비활성화"
#define NOTIFY_CLIP_FAILED			L"커서 가두기 실패"

//Menu
#define MENU_MOVE_TEXT				L"이동 활성화"
#define MENU_MOVE_TEXT_FORMAT		L"이동 활성화 (%ls)"
#define MENU_CLIP_TEXT				L"커서 가두기"
#define MENU_CLIP_TEXT_FORMAT		L"커서 가두기 (%ls)"
#define MENU_START_TEXT				L"시작 프로그램에 추가"
#define MENU_RUNTIME_TEXT			L"컨텍스트 메뉴에 런타임 체크 추가 (.exe)"
#define MENU_LOG_TEXT				L"로그"
#define MENU_CLOSE_TEXT				L"닫기"

//Link
#define LINK_STYLE					L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/window-styles"
#define LINK_EXSTYLE				L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/extended-window-styles"

//Log
#define LOG_FORMAT_NORMAL			L"%ls: %ls"
#define LOG_FORMAT_EXTRA			L"%ls: %ls(%ls)"
#define LOG_FORMAT_FILTER			L"%ls"
#define LOG_FORMAT_NW				L"%ls: %ls(%d)"
#define LOG_FORMAT_OPACITY			L"%ls: %ls(%d%%)"
#define LOG_FORMAT_HOTKEY			L"%ls"

#define LOG_CHANGE_FILTER			L"검색 필터 변경"
#define LOG_CHANGE_HOTKEY			L"단축키 변경"
#define LOG_NO_OR_NR_WINDOW			L"창이 없거나 창이 응답 없음 상태"
#define LOG_NO_PROGRAM				L"프로그램 없음"
#define LOG_SET_PROP				L"창 설정 변경"
#define LOG_CHANGE_CAPTION			L"창 제목 변경"
#define LOG_SET_OPACITY				L"창 불투명도 변경"
#define LOG_CHANGE_SCREEN			L"창 모드 변경"
#define LOG_CHANGE_SCREEN_WINDOW	L"창 모드"
#define LOG_CHANGE_SCREEN_FULL		L"전체 화면"
#define LOG_GET_COMMAND				L"프로세스 명령줄 확인"
#define LOG_CHANGE_CAPTURE			L"창 캡쳐 상태 변경"
#define LOG_SE_FAILED				L"프로그램 실행 실패"
#define LOG_OPEN_DIRECTORY			L"디렉토리 열기"

//Dialog
#define DLG_PROP_TITLE				L"속성"
#define DLG_PROP_LINK_STYLE			L"윈도우 스타일 설명 보기 (클릭)"
#define DLG_PROP_LINK_EXSTYLE		L"확장 윈도우 스타일 설명 보기 (클릭)"
#define DLG_PROP_ADD_FILTER			L"초기화"
#define DLG_PROP_CONFIRM			L"확인"
#define DLG_PROP_CANCEL				L"취소"
#define DLG_PROP_TIME				L"런타임: %d시간 %d분 %d초 %03d"
#define DLG_PROP_TIME_ACC_DENIED	L"런타임: 액세스가 거부되어 불러올 수 없습니다."
#define DLG_THREAD_TIME				L"프로세스가 이미 탐지 상태입니다."
#define DLG_THREAD_HOUR				"시간"
#define DLG_THREAD_MINUTE			"분"
#define DLG_THREAD_SECOND			"초"

//Virtual Key
#define VK_NONE						L"(없음)"