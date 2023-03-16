// FtpAppView.cpp : implementation of the FtpAppView class


#include "stdafx.h"
#include "FtpAppView.h"
#include "FtpApp.h"
#include "FtpAppDoc.h"
#include "OptionsDlg.h"
#include "Resource.h"
#include "Resources.h"


// FtpAppView

IMPLEMENT_DYNCREATE(FtpAppView, CScrView)

BEGIN_MESSAGE_MAP(FtpAppView, CScrView)
  ON_COMMAND(ID_Options, &onOptions)
END_MESSAGE_MAP()


FtpAppView::FtpAppView() noexcept {
ResourceData res;
String       pn;
  if (res.getProductName(pn)) prtNote.setTitle(pn);
  }


BOOL FtpAppView::PreCreateWindow(CREATESTRUCT& cs) {

  return CScrView::PreCreateWindow(cs);
  }


void FtpAppView::onOptions() {
OptionsDlg dlg;

  if (printer.name.isEmpty()) printer.load(0);

  initNoteOrietn();   dlg.orient = printer.toStg(prtNote.prtrOrietn);

  if (dlg.DoModal() == IDOK) {
    pMgr.setFontScale(printer.scale);

    prtNote.prtrOrietn = printer.toOrient(dlg.orient);   saveNoteOrietn();
    }
  }


// Perpare output (i.e. report) then start the output with the call to SCrView

void FtpAppView::onBeginPrinting() {prtNote.onBeginPrinting(*this);}


void FtpAppView::onDisplayOutput() {dspNote.display(*this);}


// The footer is injected into the printed output, so the output goes directly to the device.
// The output streaming functions are very similar to NotePad's streaming functions so it should not
// be a great hardship to construct a footer.

void FtpAppView::printFooter(DevBase& dev, int pageNo) {
  switch(doc()->dataSrc()) {
    case NotePadSrc : prtNote.prtFooter(dev, pageNo);  break;
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
