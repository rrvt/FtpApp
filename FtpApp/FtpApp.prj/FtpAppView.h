// FtpAppView.h : interface of the FtpAppView class


#pragma once
#include "CScrView.h"
#include "NotePadRpt.h"


class FtpAppDoc;


class FtpAppView : public CScrView {
protected: // create from serialization only

  FtpAppView() noexcept;

  DECLARE_DYNCREATE(FtpAppView)

public:

  virtual     ~FtpAppView() { }

  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual void onBeginPrinting();
  virtual void onDisplayOutput();

  virtual void printFooter(DevBase& dev, int pageNo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

  FtpAppDoc*   GetDocument() const;

public:

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:

  DECLARE_MESSAGE_MAP()

  afx_msg void onOptions();
  afx_msg void OnSetFocus(CWnd* pOldWnd);
  };


#ifndef _DEBUG  // debug version in FtpAppView.cpp
inline FtpAppDoc* FtpAppView::GetDocument() const {return reinterpret_cast<FtpAppDoc*>(m_pDocument);}
#endif

