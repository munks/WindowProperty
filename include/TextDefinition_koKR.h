#pragma once

//Window Main
#define WINDOW_MAIN_NAME			L"WindowProperty"
#define WINDOW_MAIN_CAPTION			L"Process"

//Window Log
#define WINDOW_LOG_NAME				L"WindowLog"
#define WINDOW_LOG_CAPTION			L"Log"

//Window Hotkey
#define WINDOW_HOTKEY_NAME			L"WindowHotkey"
#define WINDOW_HOTKEY_CAPTION		L"Hotkey"

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

//ID_BUTTON_ALL
#define BUTTON_FILTER_CAPTION		L"검색 필터 변경"
#define BUTTON_FILTER_TOOLTIP		L"검색할 창의 필터를 변경 (체크: 포함, 불확정 체크: 제외)"

//ID_BUTTON_CLIP
#define BUTTON_CLIP_CAPTION			L"커서 가두기"
#define BUTTON_CLIP_TOOLTIP			L"창의 영역 안으로 커서를 제한"

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
#define MENU_LOG_TEXT				L"로그"
#define MENU_CLOSE_TEXT				L"닫기"

//Link
#define LINK_STYLE					L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/window-styles"
#define LINK_EXSTYLE				L"https://learn.microsoft.com/ko-kr/windows/win32/winmsg/extended-window-styles"

//Log
#define LOG_NO_PROGRAM				L"명령줄 인수를 받아오는 프로그램이 없습니다. (%ls)"
#define LOG_SET_PROP				L"창 설정을 변경했습니다. (%ls)"
#define LOG_CHANGE_CAPTION			L"창 제목을 변경했습니다. (%ls)"
#define LOG_SET_OPACITY				L"창 불투명도를 %d%%로 설정했습니다. (%ls)"
#define LOG_CHANGE_SCREEN			L"창을 %ls로 설정했습니다. (%ls)"
#define LOG_CHANGE_SCREEN_WINDOW	L"창 모드"
#define LOG_CHANGE_SCREEN_FULL		L"전체 화면"
#define LOG_GET_COMMAND				L"프로세스의 명령줄 인수를 받아왔습니다. (%ls)"
#define LOG_CHANGE_CAPTURE			L"창의 캡쳐 가능 여부를 %ls 상태로 변경했습니다. (%ls)"
#define LOG_SE_FAILED				L"프로그램의 실행에 실패했습니다. (%ls)"
#define LOG_OPEN_DIRECTORY			L"프로그램의 경로 폴더를 열었습니다. (%ls)"
#define LOG_NO_WINDOW				L"창을 찾을 수 없습니다. (HWND: %u)"
#define LOG_CHANGE_FILTER			L"창 검색 필터를 변경했습니다."

//Dialog
#define DLG_PROP_TITLE				L"속성"
#define DLG_PROP_LINK_STYLE			L"윈도우 스타일 설명 보기 (클릭)"
#define DLG_PROP_LINK_EXSTYLE		L"확장 윈도우 스타일 설명 보기 (클릭)"
#define DLG_PROP_ADD_PROP			L"설정 고정"
#define DLG_PROP_ADD2_PROP			L"고정 해제"
#define DLG_PROP_ADD2_FILTER		L"초기화"
#define DLG_PROP_CONFIRM			L"확인"
#define DLG_PROP_CANCEL				L"취소"
#define DLG_PROP_TIME				L"런타임: %d시간 %d분 %d초 %03d"
#define DLG_PROP_TIME_ACC_DENIED	L"런타임: 액세스가 거부되어 불러올 수 없습니다."