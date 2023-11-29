//---------------------------------------------------------------------------

#ifndef MSGHELPH
#define MSGHELPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmHelp : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel3;
	TPanel *Panel4;
	TPanel *Panel5;
	TPanel *Panel6;
	TPanel *Panel7;
	TPanel *Panel8;
	TPanel *Panel9;
private:	// User declarations
public:		// User declarations
	__fastcall TfrmHelp(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmHelp *frmHelp;
//---------------------------------------------------------------------------
#endif
