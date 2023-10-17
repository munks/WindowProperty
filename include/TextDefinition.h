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
#define BUTTON_PROP_CAPTION			L"Properties"
#define BUTTON_PROP_TOOLTIP			L"Set Window Properties"

//ID_BUTTON_NAME
#define BUTTON_NAME_CAPTION			L"Change Name"
#define BUTTON_NAME_TOOLTIP			L"Change Window Name"

//ID_BUTTON_SCREEN
#define BUTTON_SCREEN_CAPTION		L"Full Screen"
#define BUTTON_SCREEN_TOOLTIP		L"Set Window To Full Screen"

//ID_BUTTON_CMD
#define BUTTON_CMD_CAPTION			L"Command Line"
#define BUTTON_CMD_TOOLTIP			L"Get Process Command Line (dllinjector Needed)"

//ID_BUTtON_OPACITY
#define BUTTON_OPACITY_CAPTION		L"Set Opacity"
#define BUTTON_OPACITY_TOOLTIP		L"Set Window's Opacity"

//ID_BUTTON_MOVE
#define BUTTON_MOVE_CAPTION			L"Move Active"
#define BUTTON_MOVE_TOOLTIP			L"Window Move Hook Active"

//ID_BUTTON_OPEN
#define BUTTON_OPEN_CAPTION			L"Open Folder"
#define BUTTON_OPEN_TOOLTIP			L"Open Execute File Directory"

//ID_BUTTON_CAPTURE
#define BUTTON_CAPTURE_CAPTION		L"Include Capture"
#define BUTTON_CAPTURE_CAPTION_2	L"Exclude Capture"
#define BUTTON_CAPTURE_TOOLTIP		L"Include/Exclude Window From Capture Screen (dllinjector Needed)"

//ID_BUTTON_HOTKEY
#define BUTTON_HOTKEY_CAPTION		L"Change Hotkey"
#define BUTTON_HOTKEY_TOOLTIP		L"Change Utilities Hotkey"

//ID_BUTTON_ALL
#define BUTTON_ALL_CAPTION			L"Show All"
#define BUTTON_ALL_TOOLTIP			L"Show Hidden Windows"

//ID_BUTTON_CLIP
#define BUTTON_CLIP_CAPTION			L"Clip Cursor"
#define BUTTON_CLIP_TOOLTIP			L"Restrict Cursor To Window"

//ID_LIST
#define LIST_TOOLTIP				L"List Of Processes"

//Change
#define CHANGE_TEXT					L"Change"

//Notify
#define NOTIFY_HOTKEY				L"Hotkey Pressed"
#define NOTIFY_MOVE_ACTIVE			L"Window Move Active"
#define NOTIFY_MOVE_DEACTIVE		L"Window Move Deactive"
#define NOTIFY_CLIP_ACTIVE			L"Clip Cursor Active"
#define NOTIFY_CLIP_DEACTIVE		L"Clip Cursor Deactive"
#define NOTIFY_CLIP_FAILED			L"Clip Cursor Failed"

//Menu
#define MENU_MOVE_TEXT				L"Move Active"
#define MENU_MOVE_TEXT_FORMAT		L"Move Active (%ls)"
#define MENU_CLIP_TEXT				L"Clip Cursor"
#define MENU_CLIP_TEXT_FORMAT		L"Clip Cursor (%ls)"
#define MENU_START_TEXT				L"Add Startup"
#define MENU_LOG_TEXT				L"Log"
#define MENU_CLOSE_TEXT				L"Close"

//Link
#define LINK_STYLE					L"https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles"
#define LINK_EXSTYLE				L"https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles"

//Log
#define LOG_NO_PROGRAM				L"Could Not Found External Program. (%ls)"
#define LOG_SET_PROP				L"Set Window Properties. (%ls)"
#define LOG_CHANGE_CAPTION			L"Changed Window Caption. (%ls)"
#define LOG_SET_OPACITY				L"Set Window Opacity To %d%%. (%ls)"
#define LOG_CHANGE_SCREEN			L"Changed Window To %ls. (%ls)"
#define LOG_CHANGE_SCREEN_WINDOW	L"Windowed Mode"
#define LOG_CHANGE_SCREEN_FULL		L"Full Screen"
#define LOG_GET_COMMAND				L"Got Program's Command Line. (%ls)"
#define LOG_CHANGE_CAPTURE			L"Set Window Capture Status To %ls. (%ls)"
#define LOG_SE_FAILED				L"Failed To Run Program. (%ls)"
#define LOG_OPEN_DIRECTORY			L"Opened Program's Directory Folder. (%ls)"