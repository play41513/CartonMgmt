//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <jpeg.hpp>
#include <Registry.hpp>
#include <pngimage.hpp>
#include <stdio.h>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <fstream>
#include <string>

#include "ConstantString.h"
#include "StringProc.h"


#define KEY_IN_BARCODE_ERROR()	{plBarcodeEnter->Font->Color = clRed;\
								edt->SetFocus();\
								edt->SelectAll();}
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid1;
	TEdit *edtBarcodeValue;
	TPanel *Panel4;
	TPanel *Panel5;
	TPanel *plModelNo1;
	TPanel *Panel17;
	TPanel *Panel18;
	TPanel *Panel19;
	TPanel *Panel20;
	TPanel *plUPC1;
	TPanel *Panel22;
	TPanel *Panel23;
	TPanel *plDOM1;
	TPanel *Panel25;
	TPanel *Panel26;
	TPanel *plQTY1;
	TPanel *Panel33;
	TPanel *Panel35;
	TPanel *plDUT;
	TPanel *plPackingInfo;
	TPanel *Panel37;
	TPanel *pledtSettingSNLast1;
	TPanel *Panel39;
	TPanel *Panel41;
	TPanel *pledtUPC1;
	TPanel *Panel43;
	TPanel *pledtDOM1;
	TPanel *Panel46;
	TPanel *pledtSettingSNStart1;
	TPanel *Panel52;
	TPanel *pledtModelNo1;
	TPanel *Panel55;
	TPanel *plPackingInfoTitle;
	TPanel *Panel58;
	TPanel *pledtSettingSNLast2;
	TPanel *Panel60;
	TPanel *Panel62;
	TPanel *pledtUPC2;
	TPanel *Panel64;
	TPanel *pledtDOM2;
	TPanel *Panel67;
	TPanel *pledtQTY2;
	TPanel *Panel70;
	TPanel *pledtSettingSNStart2;
	TPanel *Panel73;
	TPanel *pledtModelNo2;
	TPanel *Panel76;
	TPanel *plplNo1;
	TPanel *plNo1;
	TPanel *plNo2;
	TPanel *plBarcodeEnter;
	TEdit *edtModelNo1;
	TEdit *edtSettingSNStart1;
	TEdit *edtSettingSNLast1;
	TEdit *edtDOM1;
	TEdit *edtUPC1;
	TEdit *edtQTY2;
	TEdit *edtDOM2;
	TEdit *edtUPC2;
	TEdit *edtModelNo2;
	TEdit *edtSettingSNStart2;
	TEdit *edtSettingSNLast2;
	TPanel *plMain;
	TPanel *plRight;
	TPanel *Panel32;
	TPanel *plWorkOrderNumber;
	TPanel *Panel44;
	TPanel *plEmployeeID;
	TPanel *Panel50;
	TPanel *plLastCode;
	TPanel *Panel56;
	TPanel *plStartCode;
	TPanel *plWorkMode;
	TPanel *Panel28;
	TImage *Image3;
	TPanel *Panel16;
	TPanel *plPcsCtn;
	TLabel *lbErrorMsg;
	TPanel *Panel3;
	TLabel *lbErrorMsg1;
	TPanel *plBarcode;
	TImage *Image1;
	TPanel *plC_NO1;
	TPanel *pledtSetEmployeeID;
	TPanel *Panel13;
	TEdit *edtSetEmployeeID;
	TPanel *plCartonCode;
	TPanel *Panel10;
	TEdit *edtCartonCode;
	TPopupMenu *PopupMenu;
	TMenuItem *popChangID;
	TMenuItem *popPackingInfo;
	TMenuItem *popBackWOPage;
	TMenuItem *popHelp;
	TImage *ImgDisk;
	TPanel *pledtQTY1;
	TPanel *Panel49;
	TEdit *edtQTY1;
	TPanel *Panel6;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *plModelNo2;
	TPanel *plC_NO2;
	TStringGrid *StringGrid2;
	TPanel *Panel7;
	TPanel *Panel8;
	TPanel *Panel9;
	TPanel *plUPC2;
	TPanel *Panel11;
	TPanel *Panel12;
	TPanel *plDOM2;
	TPanel *Panel14;
	TPanel *Panel15;
	TPanel *plQTY2;
	TImage *Image2;
	void __fastcall StringGrid1DrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall edtBarcodeValueKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtModelNo1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtCartonCodeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall popChangIDClick(TObject *Sender);
	void __fastcall popPackingInfoClick(TObject *Sender);
	void __fastcall popBackWOPageClick(TObject *Sender);
	void __fastcall popHelpClick(TObject *Sender);
	void __fastcall plDUTDblClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);



private:	// User declarations
	void ReadRegValue();
	void SetRegValue();
	void FindLogFile();
	void ClearPackingInfo(bool Init);
	void DoubleCheckEditText(TEdit* edt);
	void CheckPackingStatus();
	void ClearPackingInfoUI();
	void EnabledPackingInfoPanel(TPanel* EnablePanel,TPanel* UnEnabledPanel);
	bool CheckCartonSNRange(AnsiString KeyIn,TEdit* edtStart,TEdit* edtLast,TPanel* plNo);
	bool writeLOG(AnsiString Msg,int GridNumber);
	bool SaveUserLogLine(AnsiString FileName,AnsiString FileLine);
	bool ReadLogFile();

	DWORD CheckBacodeValueToGrid(AnsiString BarcodValue);
	DWORD ReworkCheckBacodeValueToGrid(AnsiString BarcodValue);
	//
	AnsiString PreviousSS;
	//
	struct CellStru
	{
		TColor color;
	};
	CellStru cellbuf1[5][20];
	CellStru cellbuf2[5][20];
	//
	bool bFirstInfoPass;
	bool bLastCarton;
	struct PackingInfoStru
	{
		AnsiString ModelNo;
		AnsiString StartCode;
		AnsiString LastCode;
		AnsiString QTY;
		AnsiString DOM;
		AnsiString UPC;
		AnsiString CartonCode;
	};
	PackingInfoStru packingInfo[2];

	struct PackingStatusStru
	{
		int iStatus;
		int iCount;
		int iMaxCount;
	};
	PackingStatusStru packingStatus[4096];

public:		// User declarations
	void TfrmMain::ImportToStringGrid();
	void TfrmMain::ShowWorkOrderForm();

	AnsiString LOGFilePath,LogDiskPath,RDLOGFilePath,RDLogDiskPath;
	AnsiString astrErrorMsg;
	DWORD dwTestCount,dwCartonNo;
	bool bReWork;
	StringProc *StrProc;
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
