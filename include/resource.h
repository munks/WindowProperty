#if LANG == KOKR
	#include "TextDefinition_koKR.h"
#else
	#include "TextDefinition.h"
#endif

#define ID_MAIN					1
#define ID_ICON					2	//Icon
#define ID_LIST					3
#define ID_DLG_NAME				4	//Change Name
#define ID_DLG_PROP				5 	//Prop Change
#define ID_DLG_HOTKEY			6 	//Change Hotkey

#define ID_BUTTON_PROP			100
#define ID_BUTTON_NAME			101
#define ID_BUTTON_CMD			102
#define ID_BUTTON_OPEN			103
#define ID_BUTTON_OPACITY		104
#define ID_BUTTON_SCREEN		105
#define ID_BUTTON_MOVE			106
#define ID_BUTTON_FILTER		107
#define ID_BUTTON_CAPTURE		108
#define ID_BUTTON_CLIP			109
#define ID_BUTTON_HOTKEY		110
#define ID_BUTTON_MODULE		111
#define ID_BUTTON_ICON			130	//Menu
#define ID_BUTTON_NAME_INPUT	140
#define ID_BUTTON_NAME_CANCEL	141
#define ID_BUTTON_PROP_CONFIRM	142
#define ID_BUTTON_PROP_CANCEL	143
#define ID_BUTTON_PROP_ADD		144
#define ID_BUTTON_HK_CONFIRM	145
#define ID_BUTTON_HK_CANCEL		146

#define ID_EDIT_LOG				200
#define ID_EDIT_ALPHA			201
#define ID_EDIT_LIST			202
#define ID_EDIT_INPUT			203 //Change Name

#define ID_STATIC_PERCENTAGE	300
#define ID_STATIC_SET			301
#define ID_STATIC_STYLE			302
#define ID_STATIC_EXSTYLE		303
#define ID_STATIC_HK_MA			304
#define ID_STATIC_HK_CC			305
#define ID_STATIC_TIME			306

#define PROP_BUTTON				400
#define ID_BUTTON_STL_TM		(PROP_BUTTON + 16)
#define ID_BUTTON_STL_GM		(PROP_BUTTON + 17)
#define ID_BUTTON_STL_TS		(PROP_BUTTON + 18)
#define ID_BUTTON_STL_SYSMENU	(PROP_BUTTON + 19)
#define ID_BUTTON_STL_HSCROLL	(PROP_BUTTON + 20)
#define ID_BUTTON_STL_VSCROLL	(PROP_BUTTON + 21)
#define ID_BUTTON_STL_DLGFRAME	(PROP_BUTTON + 22)
#define ID_BUTTON_STL_BORDER	(PROP_BUTTON + 23)
#define ID_BUTTON_STL_MAXIMIZE	(PROP_BUTTON + 24)
#define ID_BUTTON_STL_CC		(PROP_BUTTON + 25)
#define ID_BUTTON_STL_CS		(PROP_BUTTON + 26)
#define ID_BUTTON_STL_DISABLED	(PROP_BUTTON + 27)
#define ID_BUTTON_STL_VISIBLE	(PROP_BUTTON + 28)
#define ID_BUTTON_STL_MINIMIZE	(PROP_BUTTON + 29)
#define ID_BUTTON_STL_CHILD		(PROP_BUTTON + 30)
#define ID_BUTTON_STL_POPUP		(PROP_BUTTON + 31)
#define ID_BUTTON_STL_PW		(PROP_BUTTON + 32) //19 | 23 | 31
#define ID_BUTTON_STL_OLW		(PROP_BUTTON + 33) //16 | 17 | 18 | 19 | 34
#define ID_BUTTON_STL_CAPTION	(PROP_BUTTON + 34) //22 | 23

#define PROP_BUTTON_EX			450
#define ID_BUTTON_EXSTL_DMF		(PROP_BUTTON_EX + 0)
#define ID_BUTTON_EXSTL_NPN		(PROP_BUTTON_EX + 2)
#define ID_BUTTON_EXSTL_TM		(PROP_BUTTON_EX + 3)
#define ID_BUTTON_EXSTL_AF		(PROP_BUTTON_EX + 4)
#define ID_BUTTON_EXSTL_TP		(PROP_BUTTON_EX + 5)
#define ID_BUTTON_EXSTL_MDIC	(PROP_BUTTON_EX + 6)
#define ID_BUTTON_EXSTL_TW		(PROP_BUTTON_EX + 7)
#define ID_BUTTON_EXSTL_WE		(PROP_BUTTON_EX + 8)
#define ID_BUTTON_EXSTL_CE		(PROP_BUTTON_EX + 9)
#define ID_BUTTON_EXSTL_CH		(PROP_BUTTON_EX + 10)
#define ID_BUTTON_EXSTL_RGH		(PROP_BUTTON_EX + 12)
#define ID_BUTTON_EXSTL_RRD		(PROP_BUTTON_EX + 13)
#define ID_BUTTON_EXSTL_LSB		(PROP_BUTTON_EX + 14)
#define ID_BUTTON_EXSTL_CP		(PROP_BUTTON_EX + 16)
#define ID_BUTTON_EXSTL_SE		(PROP_BUTTON_EX + 17)
#define ID_BUTTON_EXSTL_AW		(PROP_BUTTON_EX + 18)
#define ID_BUTTON_EXSTL_LYD		(PROP_BUTTON_EX + 19)
#define ID_BUTTON_EXSTL_NIL		(PROP_BUTTON_EX + 20)
#define ID_BUTTON_EXSTL_NRB		(PROP_BUTTON_EX + 21)
#define ID_BUTTON_EXSTL_LR		(PROP_BUTTON_EX + 22)
#define ID_BUTTON_EXSTL_CPS		(PROP_BUTTON_EX + 25)
#define ID_BUTTON_EXSTL_NA		(PROP_BUTTON_EX + 27)
#define ID_BUTTON_EXSTL_PW		(PROP_BUTTON_EX + 32) //3 | 7 | 8
#define ID_BUTTON_EXSTL_OLW		(PROP_BUTTON_EX + 33) //8 | 9

#define ID_HOTKEY_MA			500
#define ID_HOTKEY_CC			501

#define TN_MENU_MOVE			0
#define TN_MENU_CLIP			1
#define TN_MENU_INIT			2
#define TN_MENU_CLOSE			3
#define TN_MENU_LOG				4

#define TN_CLICK				100