//---------------------------------------------------------------------------

#ifndef StringProcH
#define StringProcH
#include <tchar.h>
#include <direct.h>
//---------------------------------------------------------------------------
class StringProc
{
public:
	void SetSNFormat(AnsiString String);
	AnsiString GetStringNumber(AnsiString String);
	void NewFilePath(AnsiString Path);
	int  GetSNNumber(AnsiString SN);
	bool CheckStringNumber(AnsiString String);
	bool CheckStringNotNumber(AnsiString String);
	bool CheckSNFormat(AnsiString str,bool bChangeFormat);
	bool CheckValueIsRange(AnsiString SNKeyIn,AnsiString StartCode,AnsiString LastCode);
	AnsiString GetSNString(int Number);
	AnsiString CheckDiskName(TPanel* plWorkOrderNumber);
	AnsiString SNFormat;
private:
};
#endif
