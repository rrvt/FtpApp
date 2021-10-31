// FtpApp.cpp : Defines the class behaviors for the application.
//
// This application defines only a few operations which when used properly will allow the user
// to update a web site.  Note though, that it has only been tested on Yahoo Small Business Web Host.

#include "stdafx.h"
#include "FtpApp.h"
#include "AboutDlg.h"
#include "filename.h"
#include "FtpAppDoc.h"
#include "FtpAppView.h"
#include "NotePad.h"
#include "Options.h"
#include "Site.h"


TCchar* GlobalSect   = _T("Global");
TCchar* LastSiteKey  = _T("LastSite");
TCchar* LocalWebPath = _T("LocalWebPath");


FtpApp  theApp;                       // The one and only FtpApp object
IniFile iniFile;                      // The interface to the application's initialization file


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
  bufPath  = roamPath + _T("ftpBuffer.txt");
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

  if (!ProcessShellCommand(cmdInfo)) return FALSE;            // Application initialization code follows

  setAppName(_T("FTP App")); setTitle(_T("Web Site Update"));

  view()->setFont(_T("Arial"), 12.0);

  options.load();    view()->setOrientation(options.orient);

  // Loads and displays the last manipulated by this application (if any)

  if (site.initSiteData()) {doc()->loadSiteData();   site.display();}

  m_pMainWnd->ShowWindow(SW_SHOW);   m_pMainWnd->UpdateWindow();   return TRUE;
  }



void FtpApp::OnFilePrintSetup() {
PrtrOrient orient;

  view()->setPrntrOrient(getDevMode());

    CWinApp::OnFilePrintSetup();

  orient = view()->getPrntrOrient(getDevMode());

  options.setOrient(orient);   view()->setOrientation(options.orient);
  }


int FtpApp::ExitInstance() {

  siteFtp.close();

  return CApp::ExitInstance();
  }


void FtpApp::OnHelp() {
String topic = m_pszHelpFilePath; topic += _T(">Introduction");

  ::HtmlHelp(m_pMainWnd->m_hWnd, topic,  HH_DISPLAY_TOC, 0);
  }


void FtpApp::OnAppAbout() {AboutDlg aboutDlg; aboutDlg.DoModal();}

