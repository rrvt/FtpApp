// MainFrm.cpp : implementation of the MainFrame class


#include "stdafx.h"
#include "MainFrame.h"
#include "AboutDlg.h"
#include "FtpApp.h"
#include "FtpAppDoc.h"
#include "TBBtnCtx.h"


// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWndEx)
  ON_WM_CREATE()
  ON_WM_SYSCOMMAND()

  ON_WM_MOVE()
  ON_WM_SIZE()

  ON_MESSAGE(ID_GetThrdMsg,                  &onGetThrdMsg)
  ON_MESSAGE(ID_PickThrdMsg,                 &onPickThrdMsg)
  ON_MESSAGE(ID_ConfirmMsg,                  &onConfirmMsg)

  ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &OnResetToolBar)              // MainFrame::

END_MESSAGE_MAP()


static UINT indicators[] = {
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
  };

// MainFrame construction/destruction

MainFrame::MainFrame() noexcept : isInitialized(false), stepSize(0) { }

MainFrame::~MainFrame() { }


BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs) {

  cs.style &= ~FWS_ADDTOTITLE;  cs.lpszName = _T("AddProj");         // Sets the default title left part

  return CFrameWndEx::PreCreateWindow(cs);
  }


int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
CRect winRect;

  if (CFrameWndEx::OnCreate(lpCreateStruct) == -1) return -1;

  if (!m_wndMenuBar.Create(this)) {TRACE0("Failed to create menubar\n"); return -1;}

  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  if (!toolBar.create(this, IDR_MAINFRAME)) {TRACE0("Failed to create status bar\n"); return -1;}

  addAboutToSysMenu(IDD_AboutBox, IDS_AboutBox);

  if (!m_wndStatusBar.Create(this)) {TRACE0("Failed to create status bar\n"); return -1;}
  m_wndStatusBar.SetIndicators(indicators, noElements(indicators));  //sizeof(indicators)/sizeof(UINT)

  GetWindowRect(&winRect);   winPos.initialPos(this, winRect);

  DockPane(&m_wndMenuBar);   DockPane(&toolBar);

  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
                                                                         // Affects look of toolbar, etc.
  isInitialized = true;   return 0;
  }


void MainFrame::OnSysCommand(UINT nID, LPARAM lParam) {

  if ((nID & 0xFFF0) == sysAboutID) {AboutDlg aboutDlg; aboutDlg.DoModal(); return;}

  CMainFrm::OnSysCommand(nID, lParam);
  }


void MainFrame::OnMove(int x, int y)
           {CRect winRect;   GetWindowRect(&winRect);   winPos.set(winRect);   CFrameWndEx::OnMove(x, y);}


void MainFrame::OnSize(UINT nType, int cx, int cy) {
CRect winRect;

  CFrameWndEx::OnSize(nType, cx, cy);

  if (!isInitialized) return;

  GetWindowRect(&winRect);   winPos.set(winRect);
  }


// MainFrame message handlers

afx_msg LRESULT MainFrame::OnResetToolBar(WPARAM wParam, LPARAM lParam) {setupToolBar();  return 0;}


void MainFrame::setupToolBar() { }



LRESULT MainFrame::onGetThrdMsg(WPARAM wparam, LPARAM lParam) {
FtpAppDoc& d = *doc();

  switch (wparam) {
    case ID_IncProgress : if (stepSize) incProgress();     break;
    case ID_EndThread   : d.finGetSite(lParam);     break;
    default             : d.finGetSite(TE_Unknown); break;
    }

  return 0;
  }

LRESULT MainFrame::onPickThrdMsg(WPARAM wparam, LPARAM lParam) {
FtpAppDoc& d = *doc();

  switch (wparam) {
    case ID_IncProgress : if (stepSize) incProgress();     break;
    case ID_EndThread   : d.finLoadSite(lParam);     break;
    default             : d.finLoadSite(TE_Unknown); break;
    }

  return 0;
  }


LRESULT MainFrame::onConfirmMsg(WPARAM wparam, LPARAM lParam) {
FtpAppDoc& d = *doc();

  switch (wparam) {
    case ID_IncProgress : d.cnfrmPrgs(lParam);   if (stepSize) incProgress();   break;
    case ID_EndThread   : d.finConfirm(lParam);     break;
    default             : d.finConfirm(TE_Unknown); break;
    }

  return 0;
  }


void MainFrame::openProgress(int n) {
CRect rect(0, 100, 400, 120);
CRect client;
int   width;

  GetClientRect(&client);

  width = client.right / 2;   if (n <= 0) n = 50;

  stepSize = width / n;  if (stepSize < 0) stepSize = 1;

  width = stepSize * n;

  rect.left   = (client.right     - width) / 2;
  rect.right  = rect.left         + width;
  rect.top    = client.bottom / 2 - 10;
  rect.bottom = rect.top          + 20;

  progressCtl.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect, this, IDC_ProgressCtl);
  progressCtl.SetRange(0, width);
  progressCtl.SetPos(0);
  }


// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const {CFrameWndEx::AssertValid();}

void MainFrame::Dump(CDumpContext& dc) const {CFrameWndEx::Dump(dc);}
#endif //_DEBUG

