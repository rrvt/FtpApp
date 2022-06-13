// MainFrame.h : interface of the MainFrame class


#pragma once
#include "CMainFrm.h"
#include "ToolBar.h"


class MainFrame : public CMainFrm {

CMFCMenuBar   m_wndMenuBar;
ToolBar       toolBar;
CMFCStatusBar m_wndStatusBar;

CProgressCtrl progressCtl;
int           stepSize;

protected:                                          // create from serialization only

  MainFrame() noexcept;


  DECLARE_DYNCREATE(MainFrame)

public:                                             // Overrides

  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual ~MainFrame();

  void     setupToolBar();
  ToolBar& getToolBar() {return toolBar;}

  HWND     hWnd() {return m_hWnd;}

  void     openProgress(int n);
  void     incProgress()   {progressCtl.OffsetPos(stepSize); progressCtl.Invalidate();}
  void     closeProgress() {stepSize = 0;   progressCtl.DestroyWindow();}

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:                                          // Generated message map functions

  DECLARE_MESSAGE_MAP()

  afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg LRESULT OnResetToolBar(WPARAM wParam, LPARAM lParam);

public:

  afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg LRESULT onGetThrdMsg( WPARAM wparam, LPARAM lParam);
  afx_msg LRESULT onPickThrdMsg(WPARAM wparam, LPARAM lParam);
  afx_msg LRESULT onConfirmMsg(WPARAM wparam, LPARAM lParam);
  };


