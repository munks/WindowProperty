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
#define BUTTON_FILTER_CAPTION		L"Search Filter"
#define BUTTON_FILTER_TOOLTIP		L"Change Window Search Filter (Check: Include, Indeterminate: Exclude)"

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
#define LOG_FORMAT_NORMAL			L"%ls: %ls"
#define LOG_FORMAT_EXTRA			L"%ls: %ls(%ls)"
#define LOG_FORMAT_FILTER			L"%ls"
#define LOG_FORMAT_NW				L"%ls: %ls(%d)"
#define LOG_FORMAT_OPACITY			L"%ls: %ls(%d%%)"
#define LOG_FORMAT_HOTKEY			L"%ls"

#define LOG_CHANGE_FILTER			L"Search Filter Change"
#define LOG_CHANGE_HOTKEY			L"Hotkey Change"
#define LOG_NO_WINDOW				L"No Window"
#define LOG_NO_PROGRAM				L"No Program"
#define LOG_SET_PROP				L"Window Prop Change"
#define LOG_CHANGE_CAPTION			L"Window Caption Change"
#define LOG_SET_OPACITY				L"Window Opacity Change"
#define LOG_CHANGE_SCREEN			L"Window Mode Change"
#define LOG_CHANGE_SCREEN_WINDOW	L"Windowed"
#define LOG_CHANGE_SCREEN_FULL		L"Full Screen"
#define LOG_GET_COMMAND				L"Process Command Line Check"
#define LOG_CHANGE_CAPTURE			L"Window Capture State Change"
#define LOG_SE_FAILED				L"Program Run Failed"
#define LOG_OPEN_DIRECTORY			L"Open Directory"

//Dialog
#define DLG_PROP_TITLE				L"Properties"
#define DLG_PROP_LINK_STYLE			L"See Window Style Description (Click)"
#define DLG_PROP_LINK_EXSTYLE		L"See Extended Window Style Description (Click)"
#define DLG_PROP_ADD_FILTER			L"Reset"
#define DLG_PROP_CONFIRM			L"Confirm"
#define DLG_PROP_CANCEL				L"Cancel"
#define DLG_PROP_TIME				L"Runtime: %dh %dm %ds %03d"
#define DLG_PROP_TIME_ACC_DENIED	L"Runtime: Unable to load because access denied."

//Virtual Key
#define VK_NONE						L"(None)"