//---------------------------------------------------------------------------


#pragma hdrstop
#include <vcl.h>
#include "StringProc.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
void StringProc::SetSNFormat(AnsiString String)
{
	SNFormat = String;
}
bool StringProc::CheckStringNumber(AnsiString String)
{
	if(String.Length() > 20) return false;
	TCHAR HexChar[20];

	_tcscpy_s(HexChar, 20, String.c_str());
	for(int i=0;i <String.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//非數字
		{
			return false;
		}
	}
	return true;
}
AnsiString StringProc::GetStringNumber(AnsiString String)
{
	TCHAR HexChar[20];
	AnsiString SS = "";
	_tcscpy_s(HexChar, 20, String.c_str());
	for(int i=0;i <String.Length();i++)
	{
		if(HexChar[i] >= 0x30 && HexChar[i] <= 0x39)//數字
		{
			SS += String[i];
		}
	}
	return SS;
}
bool StringProc::CheckStringNotNumber(AnsiString String)
{
	if(String.Length() > 20) return false;
	TCHAR HexChar[20];

	_tcscpy_s(HexChar, 20, String.c_str());
	for(int i=0;i <String.Length();i++)
	{
		if(HexChar[i] >= 0x30 && HexChar[i] <= 0x39)//數字
		{
			return false;
		}
	}
	return true;
}
bool StringProc::CheckSNFormat(AnsiString str,bool bChangeFormat)
{
	if(str.Length() != SNFormat.Length()) return false;

	AnsiString SS;
	if(bChangeFormat)
	{
		for(int i = 1 ; i <= SNFormat.Length() ;i++)
		{
			SS = SNFormat[i];
			if(SS == "_")
			{
				SS = str[i];
				if(!CheckStringNumber(SS)) return false;
			}
			else SNFormat[i] = str[i];
		}
	}
	else
	{
		for(int i = 1 ; i <= SNFormat.Length() ;i++)
		{
			SS = SNFormat[i];
			if(SS == "_")
			{
				SS = str[i];
				if(!CheckStringNumber(SS)) return false;
			}
			else if(SNFormat[i] != str[i]) return false;
		}
	}
	return true;
}
int StringProc::GetSNNumber(AnsiString SN)
{
	if(!CheckSNFormat(SN,false)) return 0;
	AnsiString SS = "",SS2 = "";
	for(int i = 1 ; i <= SNFormat.Length() ;i++)
	{
		SS = SNFormat[i];
		if(SS == "_")
			SS2 += SN[i];
	}
	return SS2.ToInt();
}
AnsiString StringProc::GetSNString(int Number)
{
	int NumCount = 0;
	AnsiString SS = "",SS2 = "",Temp = "";
	for(int i = 1 ; i <= SNFormat.Length() ;i++)
	{
		SS = SNFormat[i];
		if(SS == "_")
			NumCount++;
	}
	//
	AnsiString SS3 = "%0"+AnsiString(NumCount)+"d";
	Temp.printf(SS3.c_str(),Number);

	for(int i = 1 ; i <= SNFormat.Length() ;i++)
	{
		SS = SNFormat[i];
		if(SS == "_")
		{
			SS2 += Temp.SubString(1,1);
			Temp.Delete(1,1);
		}
		else SS2 += SS;
	}

	return SS2;
}
AnsiString StringProc::CheckDiskName(TPanel* plWorkOrderNumber)
{
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
	for(int i =3;i<=25;i++)
	{
		if((DiskInfo&(0x01<<i))!=0)
		{
			char Volumelabel[20];
			DWORD SerialNumber;
			DWORD MaxCLength;
			DWORD FileSysFlag;
			char FileSysName[10];
			Disk = 0x41+i;
			DiskName = AnsiString(Disk)+":\\";
			GetVolumeInformation(DiskName.c_str(),Volumelabel,255,&SerialNumber,&MaxCLength,&FileSysFlag,FileSysName,255);
			if(!strcmp(Volumelabel,DiskVerifty.c_str()))
			{
				return DiskName+"\\ASMP\\log\\"+plWorkOrderNumber->Caption+"\\CartonMangment";
			}
		}
	}
	return "";
}
void StringProc::NewFilePath(AnsiString Path)
{
	Path+="\\";
	if(!DirectoryExists(Path.c_str()))
	{
		AnsiString SS,Temp = "";
		while(true)
		{
			SS = Path.SubString(1,Path.Pos("\\"));
			Path.Delete(1,Path.Pos("\\"));
			Temp+=SS;
			_mkdir( Temp.c_str());
			if(Path == "")
				break;
		}
	}
}
bool StringProc::CheckValueIsRange(AnsiString SNKeyIn,AnsiString KeyInStartCode,AnsiString KeyInLastCode)
{
	AnsiString NowCode   = GetSNNumber(SNKeyIn);
	AnsiString LastCode  = GetSNNumber(KeyInLastCode);
	AnsiString StartCode = GetSNNumber(KeyInStartCode);
	if(StartCode.ToInt() <= NowCode.ToInt() && NowCode.ToInt() <= LastCode.ToInt())
		return true;
	return false;
}
