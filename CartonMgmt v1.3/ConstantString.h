#ifndef ConstantStringH
#define ConstantStringH
//---------------------------------------------------------------------------
//標題
const TCHAR APP_TITLE[] = _T("Carton ManagementTool v1.3 (ActionStar)");
//
#define KEY_ENTER			13
#define KEYIN_PASS_1		1
#define KEYIN_PASS_2		2
#define KEYIN_REPEAT_1		3
#define KEYIN_REPEAT_2		4
#define KEYIN_OVERRANGE		5

#define PACKING_STATUS_COMPLETE		1
#define PACKING_STATUS_STOCKOUT		2
#define PACKING_STATUS_UNFINISH		3

//ERROR MSG
AnsiString ERROR_QTY_WRONG		= "(!)Q'TY值錯誤";
AnsiString ERROR_REPEAT_SCAN 	= "(!)重複掃描";
AnsiString ERROR_OVER_RANGE  	= "(!)超出範圍";
AnsiString ERROR_FORMAT_WRONG  	= "(!)格式錯誤";
AnsiString ERROR_C_NO_WRONG  	= "(!)C/NO箱數不符合";
AnsiString ERROR_OVER_RANGE_OR_FORMAT_WRONG  	= "(!)掃描值 格式/範圍 錯誤";


#define MB_WARNING(handle, msg) MessageBox(handle, msg, MB_CAPTIONWARNING, MB_ICONWARNING)
//---------------------------------------------------------------------------
#endif
