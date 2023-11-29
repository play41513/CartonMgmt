//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MSGPackingStatus.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPackingStatus *frmPackingStatus;
//---------------------------------------------------------------------------
__fastcall TfrmPackingStatus::TfrmPackingStatus(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::FormShow(TObject *Sender)
{
	if(plBackPackingInfo->Height)
	{
		plStatus->Enabled = false;
		edtKeyIn->Text = "";
		edtKeyIn->SetFocus();
	}
	else
	{
		plStatus->Enabled = true;
		BitBtn->SetFocus();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::moUnFinishExit(TObject *Sender)
{
	BitBtn->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::Image1Click(TObject *Sender)
{
	ModalResult =	mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::Image2Click(TObject *Sender)
{
	ModalResult =	mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::edtKeyInKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key == 13)
	{
		if(edtKeyIn->Text.UpperCase()  == "[MB_OK]")
		{
			ModalResult =	mrOk;
		}
		else if(edtKeyIn->Text.UpperCase() == "[MB_CANCEL]")
		{
			ModalResult =	mrCancel;
		}
		else
			edtKeyIn->Text = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPackingStatus::FormMouseEnter(TObject *Sender)
{
	if(plBackPackingInfo->Height)
	{
		plStatus->Enabled = false;
		edtKeyIn->Text = "";
		edtKeyIn->SetFocus();
	}
	else
	{
		plStatus->Enabled = true;
		BitBtn->SetFocus();
	}
}
//---------------------------------------------------------------------------
