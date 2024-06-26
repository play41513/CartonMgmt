//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "MSGBOX.h"
#include "MSGHELP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{

}
void TfrmMain::ShowWorkOrderForm()
{
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);

	frmMsg->edtSetWorkOrderNumber->Text = plWorkOrderNumber->Caption;
	frmMsg->edtStartCode->Text = plStartCode->Caption;
	frmMsg->edtLastCode->Text = plLastCode->Caption;
	frmMsg->edtSetEmployeeID->Text = plEmployeeID->Caption.Trim();
	frmMsg->edtPcsCtn->Text	= plPcsCtn->Caption;

	if(plWorkMode->Caption.Pos("Normal"))
	{
		frmMsg->plNormalMode->Color = clLime;
		frmMsg->plReworkMode->Color = clCream;
	}
	else
	{
		frmMsg->plNormalMode->Color = clCream;
		frmMsg->plReworkMode->Color = clLime;
	}

	if(frmMsg->ShowModal()== mrOk)
	{
		plWorkOrderNumber->Caption = frmMsg->edtSetWorkOrderNumber->Text.Trim();
		plEmployeeID->Caption = frmMsg->edtSetEmployeeID->Text.Trim();
		edtSetEmployeeID->Text	=	plEmployeeID->Caption;
		plStartCode->Caption = frmMsg->edtStartCode->Text;
		plLastCode->Caption = frmMsg->edtLastCode->Text;
		plPcsCtn->Caption	= frmMsg->edtPcsCtn->Text;
		StrProc->CheckSNFormat(frmMsg->edtStartCode->Text,true);
		//
		LOGFilePath = "C:\\ASMP\\log\\"+plWorkOrderNumber->Caption+"\\CartonMangment";
		LogDiskPath = StrProc->CheckDiskName(plWorkOrderNumber);
		//
		plWorkMode->Caption = frmMsg->plNormalMode->Color == clLime ? "Normal Mode":"Rework Mode";
		if(frmMsg->plNormalMode->Color != clLime)
		{   //重工模式
			bReWork = true;
			LOGFilePath += "\\ReWork";
			if(LogDiskPath != "")
				LogDiskPath += "\\ReWork";
		}
		else	bReWork = false;

		FindLogFile();
		SetRegValue();
		CheckPackingStatus();
        lbErrorMsg1->Caption = "";
		if(bReWork)
		{
			plPackingInfo->Enabled = false;
			plPackingInfo->Width = 0;
			edtBarcodeValue->Enabled = true;
			edtBarcodeValue->SetFocus();
		}
		else
		{
			plPackingInfo->Enabled = true;
			ClearPackingInfoUI();
			plPackingInfo->Width = 885;
			pledtSetEmployeeID->Enabled  = true;
			EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
			edtModelNo1->SetFocus();
		}

		delete frmMsg;
		frmMsg = NULL;
	}
	else
	{
		delete frmMsg;
		frmMsg = NULL;
		Close();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::StringGrid1DrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
	TStringGrid * StringGrid =(TStringGrid*)Sender;

	if(StringGrid->Name.Pos("1"))
		StringGrid->Canvas->Font->Color = cellbuf1[ACol][ARow].color;
	else StringGrid->Canvas->Font->Color = cellbuf2[ACol][ARow].color;
	StringGrid->Canvas->Font->Size = 16;
	//置中
	StringGrid->Canvas->FillRect(Rect);

	DrawText(StringGrid->Canvas->Handle,
	AnsiString(StringGrid->Cells[ACol][ARow]).c_str(),
	StringGrid->Cells[ACol][ARow].Length(),
	&Rect,
	DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::edtBarcodeValueKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key == KEY_ENTER)
	{
		edtBarcodeValue->Enabled = false;
		lbErrorMsg1->Caption = "";
		if(StrProc->CheckSNFormat(edtBarcodeValue->Text,false))
		{
			if(bReWork)
			{
				ReworkCheckBacodeValueToGrid(edtBarcodeValue->Text);
				ImportToStringGrid();
				edtBarcodeValue->Enabled = true;
				edtBarcodeValue->SetFocus();
				edtBarcodeValue->SelectAll();
			}
			switch(CheckBacodeValueToGrid(edtBarcodeValue->Text))
			{
				case KEYIN_PASS_1:
					writeLOG("PASS",1);
					break;
				case KEYIN_PASS_2:
					writeLOG("PASS",2);
					break;
				case KEYIN_REPEAT_1:
					lbErrorMsg1->Caption = ERROR_REPEAT_SCAN;
					edtBarcodeValue->Font->Color = clRed;
					writeLOG(ERROR_REPEAT_SCAN,1);
					break;
				case KEYIN_REPEAT_2:
					lbErrorMsg1->Caption = ERROR_REPEAT_SCAN;
					edtBarcodeValue->Font->Color = clRed;
					writeLOG(ERROR_REPEAT_SCAN,2);
					break;
				case KEYIN_OVERRANGE:
					lbErrorMsg1->Caption = ERROR_OVER_RANGE;
					edtBarcodeValue->Font->Color = clRed;
					writeLOG(ERROR_OVER_RANGE,1);
					break;
			}
			CheckPackingStatus();
			edtBarcodeValue->Enabled = true;
			edtBarcodeValue->SetFocus();
			edtBarcodeValue->SelectAll();
		}
		else if(edtBarcodeValue->Text.Pos("[BACK_PACKINFO]") && !bReWork)
		{
			CheckPackingStatus();
			edtBarcodeValue->Text = "";
			plPackingInfo->Enabled = true;
			ClearPackingInfoUI();
			plPackingInfo->Width = 885;
			pledtSetEmployeeID->Enabled = true;
			EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
			edtModelNo1->SetFocus();
		}
		else if(edtBarcodeValue->Text.Pos("[CHANGE_EMPLOYEE_ID]") && !bReWork)
		{
			CheckPackingStatus();
			edtBarcodeValue->Text = "";
			plPackingInfo->Enabled = true;
			plPackingInfo->Width = 885;
			EnabledPackingInfoPanel(pledtSetEmployeeID,pledtModelNo1);
			edtSetEmployeeID->SetFocus();
		}
		else
		{
			lbErrorMsg1->Caption = ERROR_FORMAT_WRONG;
			edtBarcodeValue->Enabled = true;
			edtBarcodeValue->Font->Color = clRed;
			edtBarcodeValue->SetFocus();
			edtBarcodeValue->SelectAll();
        }
	}
	else if(edtBarcodeValue->Font->Color == clRed)
    	edtBarcodeValue->Font->Color = clBlack;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key== KEY_ENTER)
	{
		plBarcodeEnter->Caption  =	edtSetEmployeeID->Text;
		lbErrorMsg->Caption = "";
		if(edtSetEmployeeID->Text.Pos("[BACK_PACKINFO]"))
		{
			ClearPackingInfoUI();
			plPackingInfo->Width = 885;
			EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
			edtModelNo1->SetFocus();
		}
		else if(edtSetEmployeeID->Text.Length()== 6)
		{
			if(edtSetEmployeeID->Text.SubString(1,1).Pos("T"))
			{
				if(edtModelNo1->Text == "")
				{
					pledtModelNo1->Enabled = true;
					edtModelNo1->SetFocus();
				}
				else if(edtCartonCode->Text == "")
				{
					plCartonCode->Enabled = true;
					edtCartonCode->SetFocus();
				}
				else
				{
					pledtSetEmployeeID->Enabled  = false;
					plCartonCode->Enabled  = false;
					plEmployeeID->Caption	=	edtSetEmployeeID->Text;
					plPackingInfo->Enabled = false;
					ImportToStringGrid();
					plPackingInfo->Width = 0;
					edtBarcodeValue->Enabled = true;
					edtBarcodeValue->SetFocus();
				}
			}
			else edtSetEmployeeID->Text = "";
		}
		else if(edtSetEmployeeID->Text.Length()== 7)
		{
			if(edtSetEmployeeID->Text.SubString(1,2).Pos("SA")
				||edtSetEmployeeID->Text.SubString(1,2).Pos("UA")
				||edtSetEmployeeID->Text.SubString(1,2).Pos("UB")
				||edtSetEmployeeID->Text.SubString(1,2).Pos("XB")
				||edtSetEmployeeID->Text.SubString(1,2).Pos("XA"))
			{
				if(edtModelNo1->Text == "")
				{
					pledtModelNo1->Enabled = true;
					edtModelNo1->SetFocus();
				}
				else if(edtCartonCode->Text == "")
				{
					plCartonCode->Enabled = true;
					edtCartonCode->SetFocus();
				}
				else
				{
					pledtSetEmployeeID->Enabled  = false;
					plCartonCode->Enabled  = false;
					plEmployeeID->Caption	=	edtSetEmployeeID->Text;
					plPackingInfo->Enabled = false;
					ImportToStringGrid();
					plPackingInfo->Width = 0;
					edtBarcodeValue->Enabled = true;
					edtBarcodeValue->SetFocus();
				}
			}
			else edtSetEmployeeID->Text = "";
		}
		else edtSetEmployeeID->Text = "";
	}
}
//---------------------------------------------------------------------------
void TfrmMain::ImportToStringGrid()
{
	StringGrid1->DefaultDrawing = false;
	StringGrid2->DefaultDrawing = false;

	int iTemp1,iTemp2,iTemp1_1,iTemp2_1;
	iTemp1 = StrProc->GetSNNumber(edtSettingSNStart1->Text);
	iTemp2 = StrProc->GetSNNumber(edtSettingSNStart2->Text);
	iTemp1_1 = StrProc->GetSNNumber(edtSettingSNLast1->Text);
	iTemp2_1 = StrProc->GetSNNumber(edtSettingSNLast2->Text);
	for(int j = 0;j < 4 ; j++)
	{
		for(int i = 0;i < 5 ; i++)
		{
			if(iTemp1 <= iTemp1_1)
			{
				cellbuf1[i][j].color = clBlack;
				StringGrid1->Cells[i][j] = StrProc->GetSNString(iTemp1);
				iTemp1++;
			}
			else
			{
				StringGrid1->Cells[i][j] = "";
				cellbuf1[i][j].color = clBlack;
			}
			if(iTemp2 <= iTemp2_1)
			{
				if(iTemp1_1 != iTemp2_1)
				{
					StringGrid2->Cells[i][j] = StrProc->GetSNString(iTemp2);
					cellbuf2[i][j].color = clBlack;
					iTemp2++;
				}
				else
				{
					StringGrid2->Cells[i][j] = "";
					cellbuf2[i][j].color = clBlack;
				}
			}
			else
			{
				StringGrid2->Cells[i][j] = "";
				cellbuf2[i][j].color = clBlack;
			}
		}
	}
	plModelNo1->Caption = edtModelNo1->Text;
	plQTY1->Caption = edtQTY1->Text;
	plDOM1->Caption = edtDOM1->Text;
	plUPC1->Caption = edtUPC1->Text;
	if(iTemp2 != iTemp1)
	{
		plModelNo2->Caption = edtModelNo2->Text;
		plQTY2->Caption = edtQTY2->Text;
		plDOM2->Caption = edtDOM2->Text;
		plUPC2->Caption = edtUPC2->Text;
	}
	plEmployeeID->Caption = edtSetEmployeeID->Text;
	ReadLogFile();
}
void TfrmMain::ReadRegValue()
{
	AnsiString value;
	TRegistry *reg = new TRegistry();

	try
	{
		reg->RootKey = HKEY_CURRENT_USER;
		reg->OpenKey("SOFTWARE", false);
		bool bl = reg->OpenKey("CartonMgmt", true);
		bl = reg->OpenKey("v1.0", false);
		if(bl)
		{
			if(reg->ValueExists("WorkOrderNumber")){
				value = reg->ReadString("WorkOrderNumber");
				plWorkOrderNumber->Caption = value;
			}
			if(reg->ValueExists("PCS_CTN")){
				value = reg->ReadString("PCS_CTN");
				plPcsCtn->Caption = value;
			}
			if(reg->ValueExists("EmployeeID")){
				value = reg->ReadString("EmployeeID");
				plEmployeeID->Caption = value;
			}
			//
			if(reg->ValueExists("START_CODE")){
				value = reg->ReadString("START_CODE");
				plStartCode->Caption = value;
			}
			else plStartCode->Caption = "000000000000A";

			if(reg->ValueExists("LAST_CODE")){
				value = reg->ReadString("LAST_CODE");
				plLastCode->Caption = value;
			}
			else plLastCode->Caption = "000000000000A";
		}
	}__finally {
		delete reg;
	}
}
void TfrmMain::SetRegValue()
{
	TRegistry *reg = new TRegistry;
	AnsiString SS;
	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("SOFTWARE", false);
	bool bl = reg->OpenKey("CartonMgmt", true);
	bl = reg->OpenKey("v1.0", true);
	if(bl)
	{
		reg->WriteString("WorkOrderNumber", plWorkOrderNumber->Caption );
		reg->WriteString("PCS_CTN", plPcsCtn->Caption );
		reg->WriteString("EmployeeID", plEmployeeID->Caption );
		reg->WriteString("START_CODE", plStartCode->Caption );
		reg->WriteString("LAST_CODE", plLastCode->Caption );
	}
	delete reg;
}
void TfrmMain::FindLogFile()
{
	//
	StrProc->NewFilePath(LOGFilePath);
	StrProc->NewFilePath(LogDiskPath);
	//
	TSearchRec Sr;
	AnsiString FileName = plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";
	if(DirectoryExists(LogDiskPath.c_str()))
	{
		ImgDisk->Visible = true;
		if(FindFirst(LogDiskPath+"\\*.log",faAnyFile,Sr)==0)
		{
			do
			{
				if(Sr.Name == FileName)
				{
					LogDiskPath += "\\"+Sr.Name;
					LOGFilePath += "\\"+Sr.Name;
					break;
				}
			}
			while(FindNext(Sr)==0);
			FindClose(Sr);
		}

		if(!LogDiskPath.Pos(".log"))
		{
			LogDiskPath += "\\"+FileName;
			LOGFilePath += "\\"+FileName;
		}
		else//檔案已存在於USBDisk
		{
			if(!FileExists(LOGFilePath))
			{
				CopyFile(LogDiskPath.c_str(),LOGFilePath.c_str(),true);
			}
		}
	}
	else
	{
		ImgDisk->Visible = false;
		if(DirectoryExists(LOGFilePath.c_str()))
		{
			if(FindFirst(LOGFilePath+"\\*.log",faAnyFile,Sr)==0)
			{
				do
				{
					if(Sr.Name == FileName)
					{
						LOGFilePath += "\\"+Sr.Name;
						break;
					}
				}
				while(FindNext(Sr)==0);
				FindClose(Sr);
			}
		}
		if(!LOGFilePath.Pos(".log"))
		{
			LOGFilePath += "\\"+FileName;
		}
	}
}
void __fastcall TfrmMain::edtModelNo1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	AnsiString SS;
	TEdit * edt = (TEdit*) Sender;
	SS = edt->Text;
	PreviousSS  = plBarcodeEnter->Caption;
	bool bFirst = false;
	if(Key == KEY_ENTER)
	{
		astrErrorMsg = "";
		plBarcodeEnter->Caption = SS;
		plBarcodeEnter->Font->Color = clBlack;
		if(edt->Text.Pos("[BACK_PACKINFO]"))
		{
			ClearPackingInfoUI();
			plPackingInfo->Width = 885;
			EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
			edtModelNo1->SetFocus();
		}
		else if(edt->Name.Pos("edtModelNo"))
		{
			if(SS.Pos("-"))
			{
				if(edt->Name.Pos("1"))
				{
					EnabledPackingInfoPanel(pledtSettingSNStart1,pledtModelNo1);
					edtSettingSNStart1->SetFocus();
				}
				else
				{
					EnabledPackingInfoPanel(pledtSettingSNStart2,pledtModelNo2);
					edtSettingSNStart2->SetFocus();
				}
			}
			else
			{
				astrErrorMsg = ERROR_FORMAT_WRONG;
				KEY_IN_BARCODE_ERROR();
			}
		}
		else if(edt->Name.Pos("edtSettingSNStart"))
		{   //確認輸入是否為數字且數值不超過3位數 true : 自動填入上下限及數字欄位
			if(StrProc->CheckStringNumber(edt->Text) && edt->Text.Length()<3)
			{
				if(edt->Name.Pos("1"))
				{
					if(CheckCartonSNRange(PreviousSS,edtSettingSNStart1,edtSettingSNLast1,plNo1))
					{
						EnabledPackingInfoPanel(pledtDOM1,pledtSettingSNStart1);
						edtQTY1->Text = SS;
						edtDOM1->SetFocus();
					}
					else
						KEY_IN_BARCODE_ERROR();
				}
				else
				{
					if(CheckCartonSNRange(PreviousSS,edtSettingSNStart2,edtSettingSNLast2,plNo2))
					{
						if(plNo2->Caption == plNo1->Caption)
						{
							EnabledPackingInfoPanel(pledtDOM2,pledtSettingSNStart2);
							edtQTY2->Text = SS;
							edtDOM2->SetFocus();
						}
						else
						{
							astrErrorMsg = ERROR_C_NO_WRONG;
							KEY_IN_BARCODE_ERROR();
						}
					}
					else
						KEY_IN_BARCODE_ERROR();
				}
			}
			else
			{   //輸入確認格式及是否在範圍內
				if(!StrProc->CheckSNFormat(edt->Text,false)
				|| !StrProc->CheckValueIsRange(edt->Text,plStartCode->Caption,plLastCode->Caption))
				{
					astrErrorMsg = ERROR_OVER_RANGE_OR_FORMAT_WRONG;
					plBarcodeEnter->Font->Color = clRed;
				}
				edt->SetFocus();
				edt->SelectAll();
			}
		}
		else if(edt->Name.Pos("edtDOM"))
		{
			if(SS.Pos("-"))
			{
				if(edt->Name.Pos("1"))
				{
					EnabledPackingInfoPanel(pledtUPC1,pledtDOM1);
					edtUPC1->SetFocus();
				}
				else
				{
					EnabledPackingInfoPanel(pledtUPC2,pledtDOM2);
					edtUPC2->SetFocus();
				}
			}
			else
			{
				astrErrorMsg = ERROR_FORMAT_WRONG;
				KEY_IN_BARCODE_ERROR();
			}
		}
		else if(edt->Name.Pos("edtUPC"))
		{
			if(StrProc->CheckStringNumber(edt->Text) && edt->Text.Length()>10)
			{
				if(edt->Name.Pos("1"))
				{
					EnabledPackingInfoPanel(pledtModelNo2,pledtUPC1);
					edtModelNo2->SetFocus();
				}
				else
				{
					if(!bFirstInfoPass)
					{
						if(bLastCarton && edtSettingSNStart1->Text == edtSettingSNStart2->Text)
						{
							EnabledPackingInfoPanel(plCartonCode,pledtUPC2);
							edtCartonCode->SetFocus();
						}
						else
						{
							bFirst = true;
							EnabledPackingInfoPanel(pledtModelNo1,pledtUPC2);
							ClearPackingInfo(false);
							edtModelNo1->SetFocus();
						}
					}
					else
					{
						EnabledPackingInfoPanel(plCartonCode,pledtUPC2);
						edtCartonCode->SetFocus();
					}
				}
			}
			else
			{
				astrErrorMsg = ERROR_FORMAT_WRONG;
				KEY_IN_BARCODE_ERROR();
			}
		}
		if(bFirstInfoPass && !bFirst)
			DoubleCheckEditText(edt);
	}
	lbErrorMsg->Caption = astrErrorMsg;
}
//---------------------------------------------------------------------------
bool TfrmMain::CheckCartonSNRange(AnsiString KeyIn,TEdit* edtStart,TEdit* edtLast,TPanel* plNo)
{
	if(!StrProc->CheckSNFormat(KeyIn,false)) return false;
	int PackingMaxNum = plPcsCtn->Caption.ToInt() / 2;
	int iKeyQTY =	edtStart->Text.ToInt();
	AnsiString strTemp = "";
	AnsiString NowCode   = StrProc->GetSNNumber(KeyIn);
	AnsiString LastCode  = StrProc->GetSNNumber(plLastCode->Caption);
	AnsiString StartCode = StrProc->GetSNNumber(plStartCode->Caption);
	int Quotient = 0,numStartCode,QuotientTemp;
	if(StrProc->CheckStringNumber(LastCode) && StrProc->CheckStringNumber(StartCode))
	{
		numStartCode = StartCode.ToInt();
		if(StartCode.ToInt() <= NowCode.ToInt() && NowCode.ToInt() <= LastCode.ToInt())
		{
			//
			Quotient  = (NowCode.ToInt()-StartCode.ToInt())/ (PackingMaxNum);
			edtStart->Text = StrProc->GetSNString(numStartCode+(PackingMaxNum*Quotient));
            //計算箱數
			if(Quotient == 0) plNo->Caption = "C / NO : 1";
			else
			{
				QuotientTemp = ((numStartCode+(PackingMaxNum*Quotient))-StartCode.ToInt())+1;
				if(QuotientTemp % (PackingMaxNum*2) == 0)
					plNo->Caption = "C / NO : "+AnsiString(QuotientTemp / (PackingMaxNum*2) );
				else
					plNo->Caption = "C / NO : "+AnsiString((QuotientTemp / (PackingMaxNum*2) )+1);
			}
			//
			strTemp = numStartCode+(PackingMaxNum*(Quotient+1))-1;
			if(strTemp.ToInt() >= LastCode.ToInt())
			{   //尾數箱
				edtLast->Text = plLastCode->Caption;
				if(iKeyQTY != (StrProc->GetSNNumber(edtLast->Text)-StrProc->GetSNNumber(edtStart->Text)+1))
				{
					astrErrorMsg = ERROR_QTY_WRONG;
						return false;
				}
				if(edtStart->Name.Pos("1"))
					bLastCarton = true;
			}
			else
			{
				edtLast->Text = StrProc->GetSNString(strTemp.ToInt());
				if(iKeyQTY != (StrProc->GetSNNumber(edtLast->Text)-StrProc->GetSNNumber(edtStart->Text)+1))
				{
					astrErrorMsg = ERROR_QTY_WRONG;
						return false;
                }
			}
			//
			if(edtStart->Name.Pos("2"))
			{
				if(bLastCarton)
				{
					if(StrProc->GetSNNumber(edtSettingSNStart2->Text) != StrProc->GetSNNumber(edtSettingSNStart1->Text))
					{
						astrErrorMsg = "(!)尾箱掃描值不等於上方StartCode欄位設定值";
						return false;
					}
				}
				else
				{
					if(StrProc->GetSNNumber(edtSettingSNStart2->Text) <= StrProc->GetSNNumber(edtSettingSNStart1->Text))
					{
						astrErrorMsg = "(!)掃描值小於等於上方StartCode欄位設定值";
						return false;
					}
				}
			}
			frmMain->Refresh();
		}
		else
		{
			astrErrorMsg = ERROR_OVER_RANGE;
			return false;
		}
	}
	else return false;

	plC_NO1->Caption = plNo1->Caption;
	plC_NO2->Caption = plNo2->Caption;
	return true;
}void TfrmMain::ClearPackingInfo(bool Init)
{
	if(Init)
	{
		packingInfo[0].ModelNo = "";
		packingInfo[1].ModelNo = "";
		packingInfo[0].StartCode = "";
		packingInfo[1].StartCode = "";
		packingInfo[0].LastCode = "";
		packingInfo[1].LastCode = "";
		packingInfo[0].QTY = "";
		packingInfo[1].QTY = "";
		packingInfo[0].DOM = "";
		packingInfo[1].DOM = "";
		packingInfo[0].UPC = "";
		packingInfo[1].UPC = "";
		packingInfo[0].CartonCode = "";
		edtCartonCode->Text = "";
		edtModelNo1->Text = "";
		edtModelNo2->Text = "";
		edtSettingSNStart1->Text = "";
		edtSettingSNStart2->Text = "";
		edtSettingSNLast1->Text = "";
		edtSettingSNLast2->Text = "";
		edtQTY1->Text = "";
		edtQTY2->Text = "";
		edtDOM1->Text = "";
		edtDOM2->Text = "";
		edtUPC1->Text = "";
		edtUPC2->Text = "";
		plNo1->Caption = "C / NO :";
		plNo2->Caption = "C / NO :";
		plC_NO1->Caption = "C / NO :";
		plC_NO2->Caption = "C / NO :";
		edtCartonCode->Text = "";
		bLastCarton	   = false;
		bFirstInfoPass = false;
	}
	else
	{
		packingInfo[0].ModelNo = edtModelNo1->Text;
		packingInfo[1].ModelNo = edtModelNo2->Text;
		packingInfo[0].StartCode = edtSettingSNStart1->Text;
		packingInfo[1].StartCode = edtSettingSNStart2->Text;
		packingInfo[0].LastCode = edtSettingSNLast1->Text;
		packingInfo[1].LastCode = edtSettingSNLast2->Text;
		packingInfo[0].QTY = edtQTY1->Text;
		packingInfo[1].QTY = edtQTY2->Text;
		packingInfo[0].DOM = edtDOM1->Text;
		packingInfo[1].DOM = edtDOM2->Text;
		packingInfo[0].UPC = edtUPC1->Text;
		packingInfo[1].UPC = edtUPC2->Text;
		bFirstInfoPass = true;
	}
}
void TfrmMain::DoubleCheckEditText(TEdit* edt)
{
	bool bError = false;

	if(edt->Name.Pos("edtModelNo"))
	{
		if(edt->Name.Pos("1"))
		{
			if(edt->Text != packingInfo[0].ModelNo)
			{
				edt->Text = packingInfo[0].ModelNo;
				EnabledPackingInfoPanel(pledtModelNo1,pledtSettingSNStart1);
				edtModelNo1->SetFocus();
				edtModelNo1->SelectAll();
				bError = true;
			}
		}
		else
		{
			if(edt->Text != packingInfo[1].ModelNo)
			{
				edt->Text = packingInfo[1].ModelNo;
				EnabledPackingInfoPanel(pledtModelNo2,pledtSettingSNStart2);
				edtModelNo2->SetFocus();
				edtModelNo2->SelectAll();
				bError = true;
			}
		}
	}
	else if(edt->Name.Pos("edtSettingSNStart"))
	{
		 //輸入確認格式及是否在範圍內
		if(edt->Name.Pos("1"))
		{
			if(!StrProc->CheckSNFormat(edt->Text,false)
			|| !StrProc->CheckValueIsRange(edt->Text,packingInfo[0].StartCode,packingInfo[0].LastCode)
			|| edtQTY1->Text != packingInfo[0].QTY)
			{
					bError = true;
					edtQTY1->Text = packingInfo[0].QTY;
					edtSettingSNStart1->Text = packingInfo[0].StartCode;
					edtSettingSNLast1->Text = packingInfo[0].LastCode;
					edtSettingSNStart1->Font->Color = clBlack;
					edtSettingSNLast1->Font->Color = clBlack;
					edtQTY1->Font->Color = clBlack;
					EnabledPackingInfoPanel(pledtSettingSNStart1,pledtDOM1);
					edtSettingSNStart1->SetFocus();
					edtSettingSNStart1->SelectAll();
			}
			else
			{
				edtSettingSNLast1->Font->Color = clBlue;
				edtQTY1->Font->Color = clBlue;
			}
		}
		else
		{
			if(!StrProc->CheckSNFormat(edt->Text,false)
			|| !StrProc->CheckValueIsRange(edt->Text,packingInfo[1].StartCode,packingInfo[1].LastCode)
			|| edtQTY2->Text != packingInfo[1].QTY)
			{
					bError = true;
					edtQTY2->Text = packingInfo[1].QTY;
					edtSettingSNStart2->Text = packingInfo[1].StartCode;
					edtSettingSNLast2->Text = packingInfo[1].LastCode;
					edtSettingSNStart2->Font->Color = clBlack;
					edtSettingSNLast2->Font->Color = clBlack;
					edtQTY2->Font->Color = clBlack;
					EnabledPackingInfoPanel(pledtSettingSNStart2,pledtDOM2);
					edtSettingSNStart2->SetFocus();
					edtSettingSNStart2->SelectAll();
			}
			else
			{
				edtSettingSNLast2->Font->Color = clBlue;
				edtQTY2->Font->Color = clBlue;
            }
		}
	}
	else if(edt->Name.Pos("edtDOM"))
	{
		if(edt->Name.Pos("1"))
		{
			if(edt->Text != packingInfo[0].DOM)
			{
				edt->Text = packingInfo[0].DOM;
				EnabledPackingInfoPanel(pledtDOM1,pledtUPC1);
				edtDOM1->SetFocus();
				edtDOM1->SelectAll();
				bError = true;
			}
		}
		else
		{
			if(edt->Text != packingInfo[1].DOM)
			{
				edt->Text = packingInfo[1].DOM;
				EnabledPackingInfoPanel(pledtDOM2,pledtUPC2);
				edtDOM2->SetFocus();
				edtDOM2->SelectAll();
				bError = true;
			}
		}
	}
	else if(edt->Name.Pos("edtUPC"))
	{
		if(edt->Name.Pos("1"))
		{
			if(edt->Text != packingInfo[0].UPC)
			{
				edt->Text = packingInfo[0].UPC;
				edtUPC1->Enabled = true;
				edtUPC1->SetFocus();
				edtUPC1->SelectAll();
				bError = true;
			}
		}
		else
		{
			if(edt->Text != packingInfo[1].UPC)
			{
				edt->Text = packingInfo[1].UPC;
				edtUPC2->Enabled = true;
				edtUPC2->SetFocus();
				edtUPC2->SelectAll();
				bError = true;
			}
		}
	}


	if(!bError) edt->Font->Color = clBlue;
	else
	{
		astrErrorMsg = "(!)掃描值不符合";
		KEY_IN_BARCODE_ERROR();
	}
}

void __fastcall TfrmMain::edtCartonCodeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key== KEY_ENTER)
	{
		plBarcodeEnter->Caption  =	edtCartonCode->Text;
        lbErrorMsg->Caption = "";
		if(edtCartonCode->Text.Pos("[BACK_PACKINFO]"))
		{
			ClearPackingInfoUI();
			plPackingInfo->Width = 885;
			EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
			edtModelNo1->SetFocus();
		}
		else if(edtCartonCode->Text.Length() == 16 && StrProc->CheckStringNumber(edtCartonCode->Text))
		{
			if(packingInfo[0].CartonCode==edtCartonCode->Text)
			{
				plCartonCode->Enabled  = false;
				plPackingInfo->Enabled = false;
				ImportToStringGrid();
				plPackingInfo->Width = 0;
				edtBarcodeValue->Enabled = true;
				edtBarcodeValue->SetFocus();
			}
			else if(packingInfo[0].CartonCode == "")
			{
				packingInfo[0].CartonCode = edtCartonCode->Text;
				edtCartonCode->SelectAll();
			}
			else
			{
				astrErrorMsg = "(!)掃描值不符合上筆:"+packingInfo[0].CartonCode;
				lbErrorMsg->Caption = astrErrorMsg;
				edtCartonCode->SelectAll();
            }
		}
		else
		{
			astrErrorMsg = ERROR_FORMAT_WRONG;
			lbErrorMsg->Caption = astrErrorMsg;
			edtCartonCode->SelectAll();
		}
	}
}
//---------------------------------------------------------------------------
bool TfrmMain::writeLOG(AnsiString Msg,int GridNumber)
{
	bool bPASS[2] = {true,true};
	AnsiString SS,Temp;
	AnsiString time = FormatDateTime("mm-dd-yyyy hh:nn:ss", Now());
	TPanel* plModelNo = (TPanel *)frmMain->FindComponent("plModelNo"+AnsiString(GridNumber));
	TPanel* plQTY = (TPanel *)frmMain->FindComponent("plQTY"+AnsiString(GridNumber));
	TPanel* plDOM = (TPanel *)frmMain->FindComponent("plDOM"+AnsiString(GridNumber));
	TPanel* plUPC = (TPanel *)frmMain->FindComponent("plUPC"+AnsiString(GridNumber));
	TEdit*  edtSettingSNStart = (TEdit *)frmMain->FindComponent("edtSettingSNStart"+AnsiString(GridNumber));
	TEdit*  edtSettingSNLast = (TEdit *)frmMain->FindComponent("edtSettingSNLast"+AnsiString(GridNumber));
	Temp = ((TPanel *)frmMain->FindComponent("plNo"+AnsiString(GridNumber)))->Caption;
	AnsiString C_NO = Temp.Delete(1,9).Trim();

	if(Msg=="PASS")
	{
		dwTestCount++;
	}
	AnsiString strTemp;
	strTemp.sprintf("%04d",dwTestCount);
	if(Msg.Pos("PASS"))
	{
		SS = strTemp+" [PASS]"+time
			+" [BarcodeValue]"+edtBarcodeValue->Text;
		SS+= " [ModelNo]"+plModelNo->Caption+" [QTY]"+plQTY->Caption+" [DOM]"+plDOM->Caption+" [UPC]"+plUPC->Caption+" [C/NO]"+C_NO+" [CartonCode]"+edtCartonCode->Text;
		SS+= " [StartCode]"+edtSettingSNStart->Text+" [LastCode]"+edtSettingSNLast->Text+" [WorkOrder]"+plWorkOrderNumber->Caption+" [EmployeeID]"+plEmployeeID->Caption;
		SS += "\r\n";
	}
	else
	{
		SS =  strTemp+" [FAIL]"+time
			+" [BarcodeValue]"+edtBarcodeValue->Text;
		SS+= " [ModelNo]"+plModelNo->Caption+" [QTY]"+plQTY->Caption+" [DOM]"+plDOM->Caption+" [UPC]"+plUPC->Caption+" [C/NO]"+C_NO+" [CartonCode]"+edtCartonCode->Text;
		SS+= " [StartCode]"+edtSettingSNStart->Text+" [LastCode]"+edtSettingSNLast->Text+" [WorkOrder]"+plWorkOrderNumber->Caption	+" [EmployeeID]"+plEmployeeID->Caption;
		SS += " [ERROR]"+Msg+"\r\n";
	}
	bPASS[0] = SaveUserLogLine(LOGFilePath,SS);

	if(LogDiskPath.Pos("log") && ImgDisk->Visible )
	{
		bPASS[1] = SaveUserLogLine(LogDiskPath,SS);
		if(!bPASS[1])
		{
			ImgDisk->Visible = false;
			MessageBoxA(NULL,"偵測不到LOG隨身碟", "ERROR", MB_OK);
		}
	}
	for(int i = 0 ; i < 2 ; i++)
		if(!bPASS[i]) return false;

	return true;
}
bool  TfrmMain::SaveUserLogLine(AnsiString FileName,AnsiString FileLine)
{
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	if(NULL == fp)
		return false;
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
	return true;
}
bool  TfrmMain::ReadLogFile()
{
	AnsiString FileName = "C:\\ASMP\\LOG\\"+plWorkOrderNumber->Caption+"\\CartonMangment\\";
	AnsiString ReworkFileName	=	FileName+"ReWork\\"+plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";
	FileName += plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";
	if(FileExists(FileName))
	{
		AnsiString StartCode1 = edtSettingSNStart1->Text;
		AnsiString StartCode2 = edtSettingSNStart2->Text;
		if(StartCode2 == StartCode1) StartCode2 = "";
		StartCode1 = " [StartCode]"+StartCode1;
		if(StartCode2 != "") StartCode2 = " [StartCode]"+StartCode2;

		AnsiString BarcodeValue = "";
		ifstream lanfile(FileName.c_str());
		std::string filemsg;
		if (lanfile.is_open()) {
			while (!lanfile.eof())
			{
				getline(lanfile, filemsg);
				if (strstr(filemsg.c_str(), "PASS"))
				{
					if(strstr(filemsg.c_str(), StartCode1.c_str()))
					{
						BarcodeValue = AnsiString(filemsg.c_str());

						BarcodeValue = BarcodeValue.SubString(BarcodeValue.Pos("[BarcodeValue]"),BarcodeValue.Pos("[ModelNo]")-BarcodeValue.Pos("[BarcodeValue]"));
						BarcodeValue.Delete(1,14);
						CheckBacodeValueToGrid(BarcodeValue.Trim());
					}
					else
					{
						if(StartCode2 != "" && strstr(filemsg.c_str(), StartCode2.c_str()))
						{
							BarcodeValue = AnsiString(filemsg.c_str());
							BarcodeValue = BarcodeValue.SubString(BarcodeValue.Pos("[BarcodeValue]"),BarcodeValue.Pos("[ModelNo]")-BarcodeValue.Pos("[BarcodeValue]"));
							BarcodeValue.Delete(1,14);
							CheckBacodeValueToGrid(BarcodeValue.Trim());
						}
					}
				}
			}
			lanfile.close();
		}
	}
	if(FileExists(ReworkFileName))
	{
		AnsiString StartCode1 = edtSettingSNStart1->Text;
		AnsiString StartCode2 = edtSettingSNStart2->Text;
		if(StartCode2 == StartCode1) StartCode2 = "";
		StartCode1 = " [StartCode]"+StartCode1;
		if(StartCode2 != "") StartCode2 = " [StartCode]"+StartCode2;

		AnsiString BarcodeValue = "";
		ifstream lanfile(ReworkFileName.c_str());
		std::string filemsg;
		if (lanfile.is_open()) {
			while (!lanfile.eof())
			{
				getline(lanfile, filemsg);
				if (strstr(filemsg.c_str(), "PASS"))
				{
					if(strstr(filemsg.c_str(), StartCode1.c_str()))
					{
						BarcodeValue = AnsiString(filemsg.c_str());

						BarcodeValue = BarcodeValue.SubString(BarcodeValue.Pos("[BarcodeValue]"),BarcodeValue.Pos("[ModelNo]")-BarcodeValue.Pos("[BarcodeValue]"));
						BarcodeValue.Delete(1,14);
						CheckBacodeValueToGrid(BarcodeValue.Trim());
					}
					else
					{
						if(StartCode2 != "" && strstr(filemsg.c_str(), StartCode2.c_str()))
						{
							BarcodeValue = AnsiString(filemsg.c_str());
							BarcodeValue = BarcodeValue.SubString(BarcodeValue.Pos("[BarcodeValue]"),BarcodeValue.Pos("[ModelNo]")-BarcodeValue.Pos("[BarcodeValue]"));
							BarcodeValue.Delete(1,14);
							CheckBacodeValueToGrid(BarcodeValue.Trim());
						}
					}
				}

			}
			lanfile.close();
		}
	}
	CheckPackingStatus();
	return true;
}
DWORD TfrmMain::CheckBacodeValueToGrid(AnsiString BarcodValue)
{
	DWORD dwRETURN_VALUE = 0;
	bool bEnd = false;
	AnsiString SS1 = edtSettingSNStart1->Text;
	AnsiString SS2 = edtSettingSNStart2->Text;
	if(SS2 == SS1) SS2 = "";
	AnsiString strTemp;
	int iTemp1,iTemp2;
	iTemp1 = StrProc->GetSNNumber(SS1);
	iTemp2 = StrProc->GetSNNumber(SS2);
	for(int j = 0;j < 4 ; j++)
	{
		for(int i = 0;i < 5 ; i++)
		{
			SS1 = StrProc->GetSNString(iTemp1);
			if(SS1 == BarcodValue)
			{
				bEnd = true;
				if(cellbuf1[i][j].color != clBlue)
				{
					cellbuf1[i][j].color = clBlue;
					StringGrid1->Cells[i][j] = SS1;
					dwRETURN_VALUE = KEYIN_PASS_1;
				}
				else
				{
					dwRETURN_VALUE = KEYIN_REPEAT_1;
				}
			}
			SS2 = StrProc->GetSNString(iTemp2);
			if(SS2 == BarcodValue)
			{
				bEnd = true;
				if(cellbuf2[i][j].color != clBlue)
				{
					cellbuf2[i][j].color = clBlue;
					StringGrid2->Cells[i][j] = SS2;
					dwRETURN_VALUE = KEYIN_PASS_2;
				}
				else
				{
					dwRETURN_VALUE = KEYIN_REPEAT_2;
				}
			}
			//
			iTemp1++;
			iTemp2++;
			if(bEnd) break;
		}
		if(bEnd) break;
	}
	if(!bEnd)
	{
		dwRETURN_VALUE = KEYIN_OVERRANGE;
	}
	return dwRETURN_VALUE;
}
void TfrmMain::CheckPackingStatus()
{
	AnsiString FileName = "C:\\ASMP\\LOG\\"+plWorkOrderNumber->Caption+"\\CartonMangment\\";
	AnsiString ReworkFileName	=	FileName+"ReWork\\"+plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";
	FileName += plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";

	 //初始化參數
	int iTotalCount = StrProc->GetSNNumber(plLastCode->Caption)-StrProc->GetSNNumber(plStartCode->Caption)+1;
	int iTotalCTNCount = iTotalCount%plPcsCtn->Caption.ToInt() ? (iTotalCount/plPcsCtn->Caption.ToInt())+1 : (iTotalCount/plPcsCtn->Caption.ToInt());
	for(int i = 0 ; i < iTotalCTNCount ; i++)
	{
		packingStatus[i].iStatus =	PACKING_STATUS_UNFINISH;
		packingStatus[i].iCount  =	0;
		if(i == iTotalCTNCount-1)
		{
			if(iTotalCount%plPcsCtn->Caption.ToInt())//非整數箱
				packingStatus[i].iMaxCount = iTotalCount%plPcsCtn->Caption.ToInt();
			else
				packingStatus[i].iMaxCount  =	plPcsCtn->Caption.ToInt();
		}
		else
			packingStatus[i].iMaxCount  =	plPcsCtn->Caption.ToInt();
	}
	//讀取LOG檔
	int iC_NO = 0;
	int iCompleted =  0 , iStockout = 0 , iUnfinish = 0;
	AnsiString astrTemp = "";
	std::string filemsg;
	//
	if(FileExists(FileName))
	{
		ifstream lanfile(FileName.c_str());
		if (lanfile.is_open()) {
			while (!lanfile.eof())
			{
				getline(lanfile, filemsg);
				if (strstr(filemsg.c_str(), "PASS"))
				{
					astrTemp = AnsiString(filemsg.c_str());
					iC_NO = astrTemp.SubString(astrTemp.Pos("[C/NO]")+6,astrTemp.Pos("[CartonCode]")-astrTemp.Pos("[C/NO]")-7).ToInt();
					packingStatus[iC_NO-1].iCount++;
					if(packingStatus[iC_NO-1].iCount >= packingStatus[iC_NO-1].iMaxCount)
						packingStatus[iC_NO-1].iStatus	=	PACKING_STATUS_COMPLETE;
					else
						packingStatus[iC_NO-1].iStatus	=	PACKING_STATUS_STOCKOUT;
				}
			}
			lanfile.close();
		}
	}
	if(FileExists(ReworkFileName))
	{
		ifstream reworkfile(ReworkFileName.c_str());
		if (reworkfile.is_open()) {
			while (!reworkfile.eof())
			{
				getline(reworkfile, filemsg);
				if (strstr(filemsg.c_str(), "PASS"))
				{
					astrTemp = AnsiString(filemsg.c_str());
					iC_NO = astrTemp.SubString(astrTemp.Pos("[C/NO]")+6,astrTemp.Pos("[CartonCode]")-astrTemp.Pos("[C/NO]")-7).ToInt();
					packingStatus[iC_NO-1].iCount++;
					if(packingStatus[iC_NO-1].iCount >= packingStatus[iC_NO-1].iMaxCount)
						packingStatus[iC_NO-1].iStatus	=	PACKING_STATUS_COMPLETE;
					else
						packingStatus[iC_NO-1].iStatus	=	PACKING_STATUS_STOCKOUT;
				}
			}
			reworkfile.close();
		}
	}
	//列出
	for(int i = 0 ; i < iTotalCTNCount ; i++)
	{
		switch(packingStatus[i].iStatus)
		{
			case	PACKING_STATUS_UNFINISH:
				iUnfinish++;
				break;
			case	PACKING_STATUS_STOCKOUT:
				iStockout++;
				break;
			case	PACKING_STATUS_COMPLETE:
				iCompleted++;
				break;
		}
	}
	plDUT->Caption =	"Completed :  "+AnsiString(iCompleted)+"  Stockout  : "+AnsiString(iStockout)+"  Unfinish : "+AnsiString(iUnfinish);
}
void TfrmMain::ClearPackingInfoUI()
{
	plBarcodeEnter->Caption = "";

	edtModelNo1->Text		= "";
	edtModelNo1->Font->Color	=	clBlack;
	edtSettingSNStart1->Text	=	"";
	edtSettingSNStart1->Font->Color	=	clBlack;
	edtSettingSNLast1->Text	    =	"";
	edtSettingSNLast1->Font->Color	=	clBlack;
	edtQTY1->Text				=	"";
	edtQTY1->Font->Color	=	clBlack;
	edtDOM1->Text				=	"";
	edtDOM1->Font->Color	=	clBlack;
	edtUPC1->Text				=	"";
	edtUPC1->Font->Color	=	clBlack;
	plNo1->Caption	= "C / NO :  ";

	edtModelNo2->Text		= "";
	edtModelNo2->Font->Color	=	clBlack;
	edtSettingSNStart2->Text	=	"";
	edtSettingSNStart2->Font->Color	=	clBlack;
	edtSettingSNLast2->Text	    =	"";
	edtSettingSNLast2->Font->Color	=	clBlack;
	edtQTY2->Text				=	"";
	edtQTY2->Font->Color	=	clBlack;
	edtDOM2->Text				=	"";
	edtDOM2->Font->Color	=	clBlack;
	edtUPC2->Text				=	"";
	edtUPC2->Font->Color	=	clBlack;
	plNo2->Caption	= "C / NO :  ";

	edtCartonCode->Text			=	"";
	edtCartonCode->Font->Color	=	clBlack;
	ClearPackingInfo(true);
}
void TfrmMain::EnabledPackingInfoPanel(TPanel* EnablePanel,TPanel* UnEnabledPanel)
{
	EnablePanel->Enabled	= true;
	UnEnabledPanel->Enabled = false;
}
DWORD TfrmMain::ReworkCheckBacodeValueToGrid(AnsiString BarcodValue)
{
	AnsiString FileName = "C:\\ASMP\\LOG\\"+plWorkOrderNumber->Caption+"\\CartonMangment\\";
	AnsiString ReworkFileName	=	FileName+"ReWork\\"+plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";
	FileName += plStartCode->Caption+"-"+plLastCode->Caption+"-"+plPcsCtn->Caption+".log";

	int iTotalCount = StrProc->GetSNNumber(plLastCode->Caption)-StrProc->GetSNNumber(plStartCode->Caption)+1;
	int iTotalCTNCount = iTotalCount%plPcsCtn->Caption.ToInt() ? (iTotalCount/plPcsCtn->Caption.ToInt())+1 : (iTotalCount/plPcsCtn->Caption.ToInt());
	int iKeyInC_NO = 0;
	//找keyIn值在第幾箱
	if(StrProc->GetSNNumber(plStartCode->Caption)> StrProc->GetSNNumber(BarcodValue) || StrProc->GetSNNumber(plLastCode->Caption)< StrProc->GetSNNumber(BarcodValue) )
		return KEYIN_OVERRANGE;
	for(int i = 0 ; i < iTotalCTNCount ; i++)
	{
		if(StrProc->GetSNNumber(plStartCode->Caption)+(40*i) <= StrProc->GetSNNumber(BarcodValue)
			&&	StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+40-1 >= StrProc->GetSNNumber(BarcodValue))
		{
			iKeyInC_NO = i+1;
			edtSettingSNStart1->Text = StrProc->GetSNString(StrProc->GetSNNumber(plStartCode->Caption)+(40*i));
			if(StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+19 < StrProc->GetSNNumber(plLastCode->Caption))
			{
				edtSettingSNLast1->Text = StrProc->GetSNString(StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+19);
				edtSettingSNStart2->Text = StrProc->GetSNString(StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+20);
				if(StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+39 < StrProc->GetSNNumber(plLastCode->Caption))
					edtSettingSNLast2->Text = StrProc->GetSNString(StrProc->GetSNNumber(plStartCode->Caption)+(40*i)+39);
				else
					edtSettingSNLast2->Text = plLastCode->Caption;
			}
			else
			{
				edtSettingSNLast1->Text = plLastCode->Caption;
				edtSettingSNStart2->Text = edtSettingSNStart1->Text;
				edtSettingSNLast2->Text = edtSettingSNLast1->Text;
			}
			break;
		}
	}
	plNo1->Caption =	"C / NO : "+AnsiString(iKeyInC_NO);
	plNo2->Caption =	"C / NO : "+AnsiString(iKeyInC_NO);
	plC_NO1->Caption = "C / NO : "+AnsiString(iKeyInC_NO);
	plC_NO2->Caption = "C / NO : "+AnsiString(iKeyInC_NO);
	//讀取LOG檔取得資料
	int iC_NO = 0;
	int iCompleted =  0 , iStockout = 0 , iUnfinish = 0;
	AnsiString astrTemp = "",astrStartCode = "",astrLastCode = "",astrQTY = "",astrModelNo = "",astrDOM = "",astrUPC = "";
	bool b1 = false , b2 = false;
	std::string filemsg;
	//
	if(FileExists(FileName))
	{
		ifstream lanfile(FileName.c_str());
		if (lanfile.is_open()) {
			while (!lanfile.eof())
			{
				getline(lanfile, filemsg);
				if (strstr(filemsg.c_str(), "PASS"))
				{
					astrTemp = AnsiString(filemsg.c_str());
					iC_NO = astrTemp.SubString(astrTemp.Pos("[C/NO]")+6,astrTemp.Pos("[CartonCode]")-astrTemp.Pos("[C/NO]")-7).ToInt();
					astrStartCode = astrTemp.SubString(astrTemp.Pos("[StartCode]")+11,astrTemp.Pos("[LastCode]")-astrTemp.Pos("[StartCode]")-12);
					astrLastCode = astrTemp.SubString(astrTemp.Pos("[LastCode]")+10,astrTemp.Pos("[WorkOrder]")-astrTemp.Pos("[LastCode]")-11);
					if(iC_NO == iKeyInC_NO)
					{
						if(edtSettingSNStart1->Text == astrStartCode && !b1)
						{
							edtModelNo1->Text	= astrTemp.SubString(astrTemp.Pos("[ModelNo]")+10,astrTemp.Pos("[QTY]")-astrTemp.Pos("[ModelNo]")-11);
							edtQTY1->Text	= astrTemp.SubString(astrTemp.Pos("[QTY]")+5,astrTemp.Pos("[DOM]")-astrTemp.Pos("[QTY]")-6);
							edtDOM1->Text	= astrTemp.SubString(astrTemp.Pos("[DOM]")+5,astrTemp.Pos("[UPC]")-astrTemp.Pos("[DOM]")-6);
							edtUPC1->Text	= astrTemp.SubString(astrTemp.Pos("[UPC]")+5,astrTemp.Pos("[C/NO]")-astrTemp.Pos("[UPC]")-6);
							edtCartonCode->Text   = astrTemp.SubString(astrTemp.Pos("[CartonCode]")+12,astrTemp.Pos("[StartCode]")-astrTemp.Pos("[CartonCode]")-13);
							b1 = true;
						}
						if(edtSettingSNStart2->Text == astrStartCode && !b2)
						{
							edtModelNo2->Text	= astrTemp.SubString(astrTemp.Pos("[ModelNo]")+10,astrTemp.Pos("[QTY]")-astrTemp.Pos("[ModelNo]")-11);
							edtQTY2->Text	= astrTemp.SubString(astrTemp.Pos("[QTY]")+5,astrTemp.Pos("[DOM]")-astrTemp.Pos("[QTY]")-6);
							edtDOM2->Text	= astrTemp.SubString(astrTemp.Pos("[DOM]")+5,astrTemp.Pos("[UPC]")-astrTemp.Pos("[DOM]")-6);
							edtUPC2->Text	= astrTemp.SubString(astrTemp.Pos("[UPC]")+5,astrTemp.Pos("[C/NO]")-astrTemp.Pos("[UPC]")-6);
							edtCartonCode->Text   = astrTemp.SubString(astrTemp.Pos("[CartonCode]")+12,astrTemp.Pos("[StartCode]")-astrTemp.Pos("[CartonCode]")-13);
							b2 = true;
						}
						if(b1&&b2) break;
					}
				}
			}
			lanfile.close();
		}
	}
	if(!b1 || !b2)
	{
		if(FileExists(ReworkFileName) )
		{
			ifstream reworkfile(ReworkFileName.c_str());
			if (reworkfile.is_open()) {
				while (!reworkfile.eof())
				{
					getline(reworkfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS"))
					{
						astrTemp = AnsiString(filemsg.c_str());
						iC_NO = astrTemp.SubString(astrTemp.Pos("[C/NO]")+6,astrTemp.Pos("[CartonCode]")-astrTemp.Pos("[C/NO]")-7).ToInt();
						if(iC_NO == iKeyInC_NO)
						{
							if(edtSettingSNStart1->Text == astrStartCode && !b1)
							{
								edtModelNo1->Text	= astrTemp.SubString(astrTemp.Pos("[ModelNo]")+10,astrTemp.Pos("[QTY]")-astrTemp.Pos("[ModelNo]")-11);
								edtQTY1->Text	= astrTemp.SubString(astrTemp.Pos("[QTY]")+5,astrTemp.Pos("[DOM]")-astrTemp.Pos("[QTY]")-6);
								edtDOM1->Text	= astrTemp.SubString(astrTemp.Pos("[DOM]")+5,astrTemp.Pos("[UPC]")-astrTemp.Pos("[DOM]")-6);
								edtUPC1->Text	= astrTemp.SubString(astrTemp.Pos("[UPC]")+5,astrTemp.Pos("[C/NO]")-astrTemp.Pos("[UPC]")-6);
								edtCartonCode->Text   = astrTemp.SubString(astrTemp.Pos("[CartonCode]")+12,astrTemp.Pos("[StartCode]")-astrTemp.Pos("[CartonCode]")-13);
								b1 = true;
							}
							if(edtSettingSNStart2->Text == astrStartCode && !b2)
							{
								edtModelNo2->Text	= astrTemp.SubString(astrTemp.Pos("[ModelNo]")+10,astrTemp.Pos("[QTY]")-astrTemp.Pos("[ModelNo]")-11);
								edtQTY2->Text	= astrTemp.SubString(astrTemp.Pos("[QTY]")+5,astrTemp.Pos("[DOM]")-astrTemp.Pos("[QTY]")-6);
								edtDOM2->Text	= astrTemp.SubString(astrTemp.Pos("[DOM]")+5,astrTemp.Pos("[UPC]")-astrTemp.Pos("[DOM]")-6);
								edtUPC2->Text	= astrTemp.SubString(astrTemp.Pos("[UPC]")+5,astrTemp.Pos("[C/NO]")-astrTemp.Pos("[UPC]")-6);
								edtCartonCode->Text   = astrTemp.SubString(astrTemp.Pos("[CartonCode]")+12,astrTemp.Pos("[StartCode]")-astrTemp.Pos("[CartonCode]")-13);
								b2 = true;
							}
							if(b1&&b2) break;
						}
					}
				}
				reworkfile.close();
			}
		}
	}
	if(!b1 && !b2)
	{
		edtModelNo1->Text	= "";
		edtQTY1->Text	= "";
		edtDOM1->Text	= "";
		edtUPC1->Text	= "";
		edtModelNo2->Text	= "";
		edtQTY2->Text	= "";
		edtDOM2->Text	= "";
		edtUPC2->Text	= "";
	}
	else if(!b1 || !b2)
	{   //有疑慮
		if(!b1)
		{
			edtModelNo1->Text	= edtModelNo2->Text;
			edtQTY1->Text	= edtQTY2->Text;
			edtDOM1->Text	= edtDOM2->Text;
			edtUPC1->Text	= edtUPC2->Text;
		}
		else
		{
			edtModelNo2->Text	= edtModelNo1->Text;
			edtQTY2->Text	= edtQTY1->Text;
			edtDOM2->Text	= edtDOM1->Text;
			edtUPC2->Text	= edtUPC1->Text;
        }
	}
	return KEYIN_PASS_1;
}
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
	frmMain->Caption = APP_TITLE;
	StrProc = new StringProc();
	dwTestCount = 0;
	ReadRegValue();
	StrProc->SetSNFormat("XXXXXXX_____X");
	ShowWorkOrderForm();
	ClearPackingInfo(true);
	astrErrorMsg   = "";
	bFirstInfoPass = false;
	bLastCarton    = false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::popChangIDClick(TObject *Sender)
{
	if(!bReWork )
	{
		CheckPackingStatus();
		edtBarcodeValue->Text = "";
		plPackingInfo->Enabled = true;
		plPackingInfo->Width = 885;
		EnabledPackingInfoPanel(pledtSetEmployeeID,pledtModelNo1);
		edtSetEmployeeID->SetFocus();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::popPackingInfoClick(TObject *Sender)
{
	if(!bReWork)
	{
		CheckPackingStatus();
		edtBarcodeValue->Text = "";
		pledtSetEmployeeID->Enabled = true;
		plPackingInfo->Enabled = true;
		ClearPackingInfoUI();
		plPackingInfo->Width = 885;
		EnabledPackingInfoPanel(pledtModelNo1,plCartonCode);
		edtModelNo1->SetFocus();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::popBackWOPageClick(TObject *Sender)
{
	edtBarcodeValue->Text = "";
	edtBarcodeValue->Enabled = false;
	StrProc->SetSNFormat("XXXXXXX_____X");
	ShowWorkOrderForm();
	ClearPackingInfo(true);
	astrErrorMsg   = "";
	bFirstInfoPass = false;
	bLastCarton    = false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::popHelpClick(TObject *Sender)
{
	if(frmHelp==NULL)
		frmHelp = new TfrmHelp(Application);
	frmHelp->ShowModal();
	delete frmHelp;
	frmHelp = NULL;
}
//---------------------------------------------------------------------------

