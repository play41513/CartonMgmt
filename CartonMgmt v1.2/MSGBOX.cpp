﻿//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MSGBOX.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
__fastcall TfrmMsg::TfrmMsg(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::btnID_okClick(TObject *Sender)
{
	if(edtSetWorkOrderNumber->Text=="")
	{
		MessageBoxA(frmMsg->Handle,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
		edtSetWorkOrderNumber->SetFocus();
		edtSetWorkOrderNumber->SelectAll();
	}
	else if(edtSetEmployeeID->Text=="")
	{
		MessageBoxA(frmMsg->Handle,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
		edtSetEmployeeID->SetFocus();
		edtSetEmployeeID->SelectAll();
	}
	else if(!frmMain->StrProc->CheckSNFormat(edtStartCode->Text,true))
	{
		MessageBoxA(frmMsg->Handle,"請檢查輸入格式","WARNING", MB_ICONEXCLAMATION);
		edtStartCode->SetFocus();
		edtStartCode->SelectAll();
	}
	else if(!frmMain->StrProc->CheckSNFormat(edtLastCode->Text,true))
	{
		MessageBoxA(frmMsg->Handle,"請檢查輸入格式","WARNING", MB_ICONEXCLAMATION);
		edtLastCode->SetFocus();
		edtLastCode->SelectAll();
	}
	else if(edtPcsCtn->Text == "" || edtPcsCtn->Text == "0")
	{
		MessageBoxA(frmMsg->Handle,"請檢查入箱數","WARNING", MB_ICONEXCLAMATION);
		edtPcsCtn->SetFocus();
		edtPcsCtn->SelectAll();
	}
	else if(ImgDisk->Visible == false)
		MessageBoxA(Handle,"請確認LOG隨身碟狀態","WARNING", MB_ICONEXCLAMATION);
	else
	{
		if(CheckCodeValue())
		{
			ModalResult = mrOk;
		}
		else
		{
			MessageBoxA(frmMsg->Handle,"請檢查輸入值","WARNING", MB_ICONEXCLAMATION);
		}

	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::plSwitchClick(TObject *Sender)
{
	/*if(plSwitch->Caption=="V")
	{
		plSwitch->Caption = "^";
		frmMsg->Height = 452;
	}
	else
	{
		plSwitch->Caption = "V";
		frmMsg->Height = 269;
	}*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
	{
		btnID_ok->Click();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetWorkOrderNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
	if(Key== KEY_ENTER)
	{
		edtPcsCtn->SetFocus();
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::plNormalModeClick(TObject *Sender)
{
	TPanel* pl = (TPanel*)Sender;
	if(pl->Name.Pos("Normal"))
	{
		plNormalMode->Color = clLime;
		plReworkMode->Color = clCream;
	}
	else
	{
		plNormalMode->Color = clCream;
		plReworkMode->Color = clLime;
		//edtStartCode->Text = "000000000000";
		//edtLastCode->Text  = "FFFFFFFFFFFF";
    }
}
//---------------------------------------------------------------------------



void __fastcall TfrmMsg::edtSetEmployeeIDExit(TObject *Sender)
{
	edtSetEmployeeID->Text = edtSetEmployeeID->Text.UpperCase();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetWorkOrderNumberExit(TObject *Sender)
{
	edtSetWorkOrderNumber->Text = edtSetWorkOrderNumber->Text.UpperCase();
	AnsiString strTemp = "";
	if(edtSetWorkOrderNumber->Text.Length()== 15)
	{
		strTemp = edtSetWorkOrderNumber->Text.SubString(4,1);
		if(!strTemp.Pos("-"))
		{
			edtSetWorkOrderNumber->Text = "";
			edtSetWorkOrderNumber->SetFocus();
		}
		strTemp = edtSetWorkOrderNumber->Text.SubString(5,11);
		if(strTemp.Pos("-"))
		{
			edtSetWorkOrderNumber->Text = "";
			edtSetWorkOrderNumber->SetFocus();
		}
	}
	else
	{
		edtSetWorkOrderNumber->Text = "";
		edtSetWorkOrderNumber->SetFocus();
	}
}
//---------------------------------------------------------------------------
int TfrmMsg::HexToInt(AnsiString HexStr)
{
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // 把字串轉成全大寫

	_tcscpy_s(HexChar, 8, HexStr.c_str());
	iHex = 0;
	totalChar = HexStr.Length(); // 取得字串的長度
	for (iIndex = 0; iIndex < totalChar; iIndex++) {
		iHex <<= 4;
		if ((HexChar[iIndex] >= 0x30) && (HexChar[iIndex] <= 0x39)) {
			iHex += (HexChar[iIndex] - 0x30); // 把 0 - 9 字元轉成數字
		}
		else if ((HexChar[iIndex] >= 0x41) && (HexChar[iIndex] <= 0x46)) {
			iHex += (HexChar[iIndex] - 0x37); // ­把 A - F 字元轉成數字
		}
		else {
			iHex = 0;
			break;
		}
	}
	return iHex;
}
bool TfrmMsg::CheckCodeValue()
{
	if(edtStartCode->Text == "" || edtLastCode->Text == "")
		return false;
	/*int tmp_1 = HexToInt(edtStartCode->Text.SubString(1,2)+edtStartCode->Text.SubString(3,2)
			+edtStartCode->Text.SubString(5,2));
	int tmp_2 = HexToInt(edtLastCode->Text.SubString(1,2)+edtLastCode->Text.SubString(3,2)
			+edtLastCode->Text.SubString(5,2));
	if(tmp_2 > tmp_1)
	{
		tmp_1 = HexToInt(mskedBurntMacStart->Text.SubString(7,2)+mskedBurntMacStart->Text.SubString(9,2)
				+mskedBurntMacStart->Text.SubString(11,2));
		tmp_2 = HexToInt(mskedBurntMacLast->Text.SubString(7,2)+mskedBurntMacLast->Text.SubString(9,2)
				+mskedBurntMacLast->Text.SubString(11,2));
		if(tmp_2 >= tmp_1)
			return true;
		else return false;
	}
	else if(tmp_2 == tmp_1)
	{
		tmp_1 = HexToInt(mskedBurntMacStart->Text.SubString(7,2)+mskedBurntMacStart->Text.SubString(9,2)
				+mskedBurntMacStart->Text.SubString(11,2));
		tmp_2 = HexToInt(mskedBurntMacLast->Text.SubString(7,2)+mskedBurntMacLast->Text.SubString(9,2)
				+mskedBurntMacLast->Text.SubString(11,2));
		if(tmp_2 > tmp_1)
			return true;
		else return false;
	}
	else return false;*/
	return true;
}



void __fastcall TfrmMsg::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(ModalResult != mrOk)
		ModalResult = mrNo;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
	ImgDisk->Visible = false;
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
				ImgDisk->Visible = true;
				break;
			}
		}
	}
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtStartCodeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key== KEY_ENTER)
	{
		if(frmMain->StrProc->CheckSNFormat(edtStartCode->Text,true))
			edtLastCode->SetFocus();
		else
		{
			edtStartCode->SetFocus();
			edtStartCode->SelectAll();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtLastCodeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key== KEY_ENTER)
	{
		if(frmMain->StrProc->CheckSNFormat(edtLastCode->Text,false))
			edtSetEmployeeID->SetFocus();
		else
		{
			edtLastCode->SetFocus();
			edtLastCode->SelectAll();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtPcsCtnKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key== KEY_ENTER)
	{
		if(frmMain->StrProc->CheckStringNumber(edtPcsCtn->Text))
		{
			if(plNormalMode->Color == clLime)
				edtStartCode->SetFocus();
			else
				edtSetEmployeeID->SetFocus();
		}
		else
        	edtPcsCtn->SelectAll();
	}
}
//---------------------------------------------------------------------------
