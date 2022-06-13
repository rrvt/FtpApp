// FtpAppView.cpp : implementation of the FtpAppView class


#include "stdafx.h"
#include "FtpAppView.h"
#include "FtpApp.h"
#include "FtpAppDoc.h"
#include "Options.h"
#include "Resources.h"


// FtpAppView

IMPLEMENT_DYNCREATE(FtpAppView, CScrView)

BEGIN_MESSAGE_MAP(FtpAppView, CScrView)
END_MESSAGE_MAP()


FtpAppView::FtpAppView() noexcept : dspNote( dMgr.getNotePad()), prtNote( pMgr.getNotePad()) {
ResourceData res;
String       pn;
  if (res.getProductName(pn)) prtNote.setTitle(pn);
  }


BOOL FtpAppView::PreCreateWindow(CREATESTRUCT& cs) {

  return CScrView::PreCreateWindow(cs);
  }


void FtpAppView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) {
uint   x;
double topMgn   = options.topMargin.stod(x);
double leftMgn  = options.leftMargin.stod(x);
double rightMgn = options.rightMargin.stod(x);
double botMgn   = options.botMargin.stod(x);

  setMgns(leftMgn,  topMgn,  rightMgn, botMgn, pDC);   CScrView::OnPrepareDC(pDC, pInfo);
  }


// Perpare output (i.e. report) then start the output with the call to SCrView

void FtpAppView::onPrepareOutput(bool printing) {
DataSource ds = doc()->dataSrc();

  if (printing)
    switch(ds) {
      case NotePadSrc : prtNote.print(*this);  break;
      }

  else
    switch(ds) {
      case NotePadSrc : dspNote.display(*this);  break;
      }


  CScrView::onPrepareOutput(printing);
  }


void FtpAppView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) {

  switch(doc()->dataSrc()) {
    case NotePadSrc : setOrientation(options.orient); break;    // Setup separate Orientation?
    case StoreSrc   : setOrientation(options.orient); break;
    }
  setPrntrOrient(theApp.getDevMode(), pDC);   CScrView::OnBeginPrinting(pDC, pInfo);
  }


// The footer is injected into the printed output, so the output goes directly to the device.
// The output streaming functions are very similar to NotePad's streaming functions so it should not
// be a great hardship to construct a footer.

void FtpAppView::printFooter(Device& dev, int pageNo) {
  switch(doc()->dataSrc()) {
    case NotePadSrc : prtNote.footer(dev, pageNo);  break;
    }
  }



void FtpAppView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {

  CScrView::OnEndPrinting(pDC, pInfo);

  switch(doc()->dataSrc()) {
    case NotePadSrc : break;
    case StoreSrc   : break;
    }
  }


void FtpAppView::OnSetFocus(CWnd* pOldWnd) {

  CScrView::OnSetFocus(pOldWnd);

  switch(doc()->dataSrc()) {
    case NotePadSrc : break;
    case StoreSrc   : break;
    }
  }


// FtpAppView diagnostics

#ifdef _DEBUG

void FtpAppView::AssertValid() const {CScrollView::AssertValid();}

void FtpAppView::Dump(CDumpContext& dc) const {CScrollView::Dump(dc);}
                                             // non-debug version is inline
FtpAppDoc* FtpAppView::GetDocument() const
  {ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(FtpAppDoc))); return (FtpAppDoc*)m_pDocument;}

#endif //_DEBUG
