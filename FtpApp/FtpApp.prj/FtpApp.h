// FtpApp.h : main header file for the FtpApp application

#pragma once
#include "CApp.h"
#include "MainFrame.h"


class FtpAppDoc;
class FtpAppView;


// FtpApp:
// See FtpApp.cpp for the implementation of this class

class FtpApp : public CApp {

String roamPath;
String appPath;

public:

               FtpApp() noexcept : CApp(this) { }

  virtual BOOL InitInstance();
  virtual int  ExitInstance();

  FtpAppDoc*   doc()  {return (FtpAppDoc*)  CApp::getDoc();}
  FtpAppView*  view() {return (FtpAppView*) CApp::getView();}
  MainFrame*   mainFrm() {return (MainFrame*) m_pMainWnd;}
  String&      roamingPath() {return roamPath;}
  String&      thisAppPath() {return appPath;}

  DECLARE_MESSAGE_MAP()

  afx_msg void OnAppAbout();
  afx_msg void OnHelp();
  };


extern FtpApp theApp;

inline void        invalidate()  {theApp.invalidate();}
inline FtpAppDoc*  doc()         {return theApp.doc();}
inline FtpAppView* view()        {return theApp.view();}
inline MainFrame*  mainFrm()     {return theApp.mainFrm();}
inline ToolBar&    getToolBar()  {return mainFrm()->getToolBar();}
inline String&     roamingPath() {return theApp.roamingPath();}
inline String&     appPath()     {return theApp.thisAppPath();}

