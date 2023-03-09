// FtpApp.cpp : Defines the class behaviors for the application.


#include "stdafx.h"
#include "FtpApp.h"
#include "AboutDlg.h"
#include "ExtraResource.h"
#include "filename.h"
#include "Ftp.h"
#include "IniFile.h"
#include "MainFrame.h"
#include "NotePad.h"
#include "FtpAppDoc.h"
#include "FtpAppView.h"


FtpApp  theApp;                       // The one and only FtpApp object
IniFile iniFile;


// FtpApp

BEGIN_MESSAGE_MAP(FtpApp, CWinAppEx)
  ON_COMMAND(ID_FILE_PRINT_SETUP, &OnFilePrintSetup)
  ON_COMMAND(ID_Help,             &OnHelp)
  ON_COMMAND(ID_App_About,        &OnAppAbout)
END_MESSAGE_MAP()


// FtpApp initialization

BOOL FtpApp::InitInstance() {

  CWinAppEx::InitInstance();

  iniFile.setAppDataPath(m_pszHelpFilePath, *this);

  roamPath = getPath(iniFile.getAppDataPath(m_pszHelpFilePath));
  appPath  = getPath(m_pszHelpFilePath);

  notePad.clear();

  SetRegistryKey(appID);

  LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

  // Register the application's document templates.  Document templates
  // serve as the connection between documents, frame windows and views

  CSingleDocTemplate* pDocTemplate;

  pDocTemplate = new CSingleDocTemplate(
    IDR_MAINFRAME,
    RUNTIME_CLASS(FtpAppDoc),
    RUNTIME_CLASS(MainFrame),       // main SDI frame window
    RUNTIME_CLASS(FtpAppView));

  if (!pDocTemplate) return FALSE;

  AddDocTemplate(pDocTemplate);

//  EnableShellOpen(); RegisterShellFileTypes(TRUE);       // Enable DDE Execute open

  // Parse command line for standard shell commands, DDE, file open

  CCommandLineInfo cmdInfo;   ParseCommandLine(cmdInfo);

  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.

  if (!ProcessShellCommand(cmdInfo)) return FALSE;

  setAppName(_T("Ftp Test")); setTitle(_T("Main Window"));

  view()->setFont(_T("Arial"), 12.0);

  m_pMainWnd->ShowWindow(SW_SHOW);   m_pMainWnd->UpdateWindow();   return TRUE;
  }


int FtpApp::ExitInstance() {

  ftp.close();

#ifdef DebugMemoryLeaks
  _CrtDumpMemoryLeaks();
#endif

  return CApp::ExitInstance();
  }


void FtpApp::OnHelp() {
String topic = m_pszHelpFilePath; topic += _T(">Introduction");

  ::HtmlHelp(m_pMainWnd->m_hWnd, topic,  HH_DISPLAY_TOC, 0);
  }


void FtpApp::OnAppAbout() {AboutDlg aboutDlg; aboutDlg.DoModal();}

