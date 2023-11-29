//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("MSGBOX.cpp", frmMsg);
USEFORM("main.cpp", frmMain);
USEFORM("MSGHELP.cpp", frmHelp);
USEFORM("MSGPackingStatus.cpp", frmPackingStatus);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmMsg), &frmMsg);
		Application->CreateForm(__classid(TfrmHelp), &frmHelp);
		Application->CreateForm(__classid(TfrmPackingStatus), &frmPackingStatus);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
