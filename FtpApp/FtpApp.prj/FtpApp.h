// FtpApp.h : main header file for the FtpApp application

#pragma once
#include "CApp.h"
#include "IniFile.h"


extern TCchar* GlobalSect;
extern TCchar* LastSiteKey;
extern TCchar* LocalWebPath;

extern IniFile iniFile;


class FtpAppDoc;
class FtpAppView;
class MainFrame;


// FtpApp:
// See FtpApp.cpp for the implementation of this class

class FtpApp : public CApp {

String bufPath;
String roamPath;
String appPath;

public:

               FtpApp() noexcept : CApp(this) { }

  virtual BOOL InitInstance();
  virtual int  ExitInstance();

          FtpAppDoc*  doc()         {return (FtpAppDoc*)  CApp::getDoc();}
          FtpAppView* view()        {return (FtpAppView*) CApp::getView();}
          MainFrame*  mainFrm()     {return (MainFrame*) m_pMainWnd;}
          String&     roamingPath() {return roamPath;}
          String&     ftpBufPath()  {return bufPath;}
          String&     thisAppPath() {return appPath;}

  DECLARE_MESSAGE_MAP()

  afx_msg void OnFilePrintSetup();
  afx_msg void OnAppAbout();
  afx_msg void OnHelp();
  };


extern FtpApp theApp;

inline void        invalidate() {theApp.invalidate();}
inline FtpAppDoc*  doc()        {return theApp.doc();}
inline FtpAppView* view()       {return theApp.view();}

