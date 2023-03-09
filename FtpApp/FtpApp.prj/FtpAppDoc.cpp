 // FtpAppDoc.cpp : implementation of the FtpAppDoc class


#include "stdafx.h"
#include "FtpAppDoc.h"
#include "ExtraResource.h"
#include "filename.h"
#include "Ftp.h"
#include "FtpAppView.h"
#include "FtpTransport.h"
#include "GetPathDlg.h"
#include "MessageBox.h"
#include "NotePad.h"
#include "Resource.h"
#include "SiteFiles.h"
#include "SiteUpdateDlg.h"
#include "ToolBar.h"
#include "Utilities.h"


static TCchar* LocalWebPath = _T("LocalWebPath");


static UINT fromWebThrd(LPVOID param);
static UINT confirmThrd(LPVOID param);


// FtpAppDoc

IMPLEMENT_DYNCREATE(FtpAppDoc, CDoc)

BEGIN_MESSAGE_MAP(FtpAppDoc, CDoc)
  ON_COMMAND(ID_NewSite,        &OnNewSite)
  ON_COMMAND(ID_EditSite,       &onEditSite)
  ON_COMMAND(ID_PickSite,       &onPickSite)
  ON_COMMAND(ID_UpdateSite,     &OnUpdateSite)
  ON_COMMAND(ID_GetSite,        &onGetSite)
  ON_COMMAND(ID_ConfirmUpdate,  &OnConfirmUpdate)
  ON_COMMAND(ID_DeleteSite,     &onDeleteSite)
END_MESSAGE_MAP()


// FtpAppDoc construction/destruction

FtpAppDoc::FtpAppDoc() noexcept : dataSource(NotePadSrc), cmdLock(false), savePrv(false) { }

FtpAppDoc::~FtpAppDoc() { }


void FtpAppDoc::OnNewSite() {

  if (cmdLock) return;

  notePad.clear();   iniFile.readString(GlobalSect, LocalWebPath, path);

  getDirPathDlg(_T("Web Site"), path);   fixRootPath(path);   //lclSite.rootPath = path;

  String pth = ::getPath(path);

  iniFile.writeString(GlobalSect, LocalWebPath, pth);

  siteID.root = pth;   siteID.edit();

  loadSite();
  }


void FtpAppDoc::onEditSite() {

  if (cmdLock) return;

  notePad.clear();   siteID.edit();   loadSite();
  }


void FtpAppDoc::onPickSite() {
FtpChIter iter(transport);
String    line;

  if (cmdLock) return;

  notePad.clear();   siteID.pickSite();   loadSite();
  }


void FtpAppDoc::loadSite() {

  ftp.close();

  if (!siteID.login()) {notePad << _T("Not logged in") << nCrlf; display(NotePadSrc); return;}

  curFiles.clear();
  curFiles.setRoot(siteID.root);
  curFiles.fromPC(siteID.root);

  savePrv = false;

  if (!loadPrevSiteData()) {

    savePrv = true;

//  cmdLock = true;   mainFrm()->openProgress(siteID.noWebFiles);

    startWkrThrd(fromWebThrd, ID_PickThrdMsg, siteID.noWebFiles);   return;
    }

  finLoadSite(TE_Normal);
  }


void FtpAppDoc::finLoadSite(LPARAM lParam) {

  if (!finWkrThrd(lParam)) return;

  siteID.noWebFiles = prvFiles.nData();

  prvFiles.setRoot(siteID.root);       // Transform list into local file list to facilitate comparisons

  if (savePrv) saveSite(PrvSiteSrc);   savePrv = false;

  prvFiles.display(_T("Previous"));
  curFiles.display(_T("Current"));

  display(NotePadSrc);
  }


// Start the update process by finding all the local and remote web site files and doing some
// comparisons of the "current" list and the "former" or "older" list.

void FtpAppDoc::OnUpdateSite() {
FilesIter iter(curFiles);
SiteFile* sf;

  if (cmdLock) return;

  notePad.clear();   updateFiles.clear();

  for (sf = iter(); sf; sf = iter++) {

    SiteFile* uf = updateFiles.data = *sf;

    cmpPrvFile(*uf);
    }

  findDeleted();   dspUpdates();
  }


void FtpAppDoc::cmpPrvFile(SiteFile& uf) {
SiteFile* pf = prvFiles.find(uf.path);

  if (!pf || pf->size != uf.size || uf.date > pf->date) uf.status = PutSts;
  }


void FtpAppDoc::findDeleted() {
FilesIter iter(prvFiles);
SiteFile* pf;

  for (pf = iter(); pf; pf = iter++) {
    if (!updateFiles.find(pf->path)) {pf->status = DelSts;  updateFiles.data = *pf;}
    }
  }


void FtpAppDoc::onGetSite() {

  if (cmdLock) return;

  notePad.clear();

  siteID.pickSite();

  if (!siteID.login()) {notePad << _T("Not logged in") << nCrlf; display(NotePadSrc); return;}

  curFiles.clear();
  curFiles.setRoot(siteID.root);
  curFiles.fromPC(siteID.root);

//cmdLock = true;   mainFrm()->openProgress(siteID.noWebFiles);

  startWkrThrd(fromWebThrd, ID_GetThrdMsg, siteID.noWebFiles);
  }


void FtpAppDoc::finGetSite(LPARAM lParam) {
FilesIter iter(prvFiles);
SiteFile* pf;
SiteFile* uf;
String    root = siteID.root;

  if (!finWkrThrd(lParam)) return;

  siteID.noWebFiles = prvFiles.nData();    siteID.saveSiteData();

  for (pf = iter(); pf; pf = iter++) {

    uf = updateFiles.data = *pf;   uf->status = GetSts;

    String path = root + uf->path;

    uf->addLclAttr(path);
    }

  updateFiles.setRoot(siteID.root);    saveSite(UpdSiteSrc);

  dspUpdates();
  }


UINT fromWebThrd(LPVOID param) {
HWND hWnd  = mainFrm()->m_hWnd;
uint msgID = (uint) param;
uint rslt;

  try {prvFiles.startFromWeb(siteID.webRoot, hWnd, msgID);   rslt = TE_Normal;}
  catch (...) {                                              rslt = TE_Exception;}

  ::PostMessage(hWnd, msgID, ID_EndThread, rslt);   return 0;
  }


// Lock the commands and start the progress bar and the thread

void FtpAppDoc::startWkrThrd(AFX_THREADPROC thdProc, uint arg, int n)
            {cmdLock = true;   mainFrm()->openProgress(n);   AfxBeginThread(thdProc, LPVOID(arg), 0, 0);}


// Finish the thread unlocking the commands, closing the progress bar posting a message about the results

bool FtpAppDoc::finWkrThrd(LPARAM lParam) {
bool rslt;

  switch (lParam) {
    case TE_Normal    : notePad << _T("Finished successfully");     rslt = true;  break;
    case TE_Exception : notePad << _T("Thread had an exception");   rslt = false; break;
    default           : notePad << _T("Unknown Thread completion"); rslt = false; break;
    }

  notePad << nCrlf;   mainFrm()->closeProgress();   cmdLock = false;   display(NotePadSrc);

  return rslt;
  }


// Present the list of files for which some operation is indicated by the "Update" function above.
// Allow the user to select those updates to perform (or select do all updates).  Then perform
// the operations, update the database (list of files, etc) and save the database.

void FtpAppDoc::OnConfirmUpdate() {
SiteUpdateDlg dlg(updateFiles);
FilesIter     iter(updateFiles);
SiteFile*     uf;
int           count;

  if (cmdLock) return;

  notePad.clear();

  for (count = 0, uf = iter(); uf; uf = iter++) if (uf->check) count++;

  if (dlg.DoModal() == IDOK) {

//    cmdLock = true;   mainFrm()->openProgress(count);

    startWkrThrd(confirmThrd, ID_ConfirmMsg, count);
    }
  }


void FtpAppDoc::finConfirm(LPARAM lParam) {

  if (!finWkrThrd(lParam)) return;

  if (savePrv) saveSite(PrvSiteSrc);
  }


UINT confirmThrd(LPVOID param) {
HWND hWnd  = mainFrm()->m_hWnd;
uint rslt;

  try {doc()->confirmUpdate();   rslt = TE_Normal;}
  catch (...) {                  rslt = TE_Exception;}

  ::PostMessage(hWnd, ID_ConfirmMsg, ID_EndThread, rslt);   return 0;
  }


void FtpAppDoc::confirmUpdate() {
FilesIter iter(updateFiles);
SiteFile* uf;

  savePrv = false;

  for (uf = iter(); uf; uf = iter++) {

    if (!uf->check) continue;

    switch (uf->status) {

      case NilSts : break;

      case PutSts : if (put(*uf)) {prvFiles.update(*uf);   savePrv = true;}  break;

      case DelSts : if (del(*uf))
                        {prvFiles.delRcd(*uf);   updateFiles.delRcd(*uf);   savePrv = true;}   break;

      case GetSts : if (get(*uf))
                           {curFiles.update(*uf);   prvFiles.update(*uf);   savePrv = true;}   break;

      default     : continue;  // post am error message here
      }

    ::PostMessage(mainFrm()->m_hWnd, ID_ConfirmMsg, ID_IncProgress, iter.index());
    }
  }


bool FtpAppDoc::put(SiteFile& uf) {
String src = siteID.root    + uf.path;
String dst = siteID.webRoot + uf.path;

  if (!transport.load(src)) return false;

  return ftp.put(remotePath(dst));
  }


bool FtpAppDoc::get(SiteFile& uf) {
String src = siteID.webRoot + uf.path;
String dst = siteID.root    + uf.path;

  if (!ftp.get(remotePath(src))) return false;

  return transport.store(dst);
  }


bool FtpAppDoc::del(SiteFile& uf)
                              {String dst = siteID.webRoot + uf.path;   return ftp.del(remotePath(dst));}


void FtpAppDoc::cnfrmPrgs(LPARAM lParam) {
int       i  = lParam;
SiteFile* sf = updateFiles.datum(i);   if (!sf) return;

  notePad << nClrTabs << nSetRTab(32) << nSetRTab(43) << nSetTab(45);

  sf->display();   display(NotePadSrc);
  }


void FtpAppDoc::dspUpdates(bool onlyChkd) {
FilesIter iter(updateFiles);
SiteFile* uf;
SiteFlSts status;
SiteFile* pf;

  notePad << _T("Update List") << nCrlf << nCrlf;

  notePad << nSetRTab(20) << nSetRTab(32) << nSetTab(35);

  for (uf = iter(); uf; uf = iter++) {

    if (onlyChkd && !uf->check) continue;

    status = uf->status;   pf = prvFiles.find(uf->path);

    switch (status) {
      case PutSts : notePad << _T("Put File:    "); break;
      case GetSts : notePad << _T("Get File:    "); break;
      case DelSts : notePad << _T("Delete File: "); break;
      default     : notePad << _T("Unchanged:   "); break;
      }

    notePad << nTab << uf->size << nTab << uf->date;
    notePad << nTab << uf->path << nCrlf;

    if (pf && status == PutSts) notePad << nTab << pf->size << nTab << pf->date << nCrlf;
    }

  display(NotePadSrc);
  }


void FtpAppDoc::onDeleteSite() {
String path;

  if (cmdLock) return;

  notePad.clear();

  siteID.pickSite();

  if (!siteID.delCurSite()) return;

  if (!dbPath(path).isEmpty()) DeleteFile(path);
  }


void FtpAppDoc::display(DataSource ds) {dataSource = ds; invalidate();}


// The load and save functions do the necessary things to open files and then call the serialize
// function which switches to the correct operation and function to do the work.  This was the MFC
// way of doing business and I just copied with my own flavor of opening files, etc...

bool FtpAppDoc::loadPrevSiteData() {
String path;
bool   rslt;

  dataSource = PrvSiteSrc;   rslt = OnOpenDocument(dbPath(path));   dataSource = NotePadSrc;  return rslt;
  }


void FtpAppDoc::saveSite(DataSource src) {
String path;

  dataSource = src;   onSaveDocument(dbPath(path));   dataSource = NotePadSrc;
  }



// UglyDoc serialization

void FtpAppDoc::serialize(Archive& ar) {

  if (ar.isStoring())
    switch(dataSource) {
      case NotePadSrc : notePad.archive(ar);  return;
      case PrvSiteSrc : prvFiles.save(ar);    return;
      case LclSiteSrc : curFiles.save(ar);    return;
      case UpdSiteSrc : updateFiles.save(ar); return;
      default         : return;
      }

  else
    switch(dataSource) {
      case PrvSiteSrc : prvFiles.load(ar);   return;
      default         : return;
      }
  }


// FtpAppDoc diagnostics

#ifdef _DEBUG
void FtpAppDoc::AssertValid() const {CDocument::AssertValid();}
void FtpAppDoc::Dump(CDumpContext& dc) const {CDocument::Dump(dc);}
#endif //_DEBUG



#if 0
void FtpAppDoc::cnfrmStsErr(LPARAM lParam) {
  notePad << _T("*** Something Happened:") << nCrlf;
  cnfrmPrgs(lParam);
  }
#endif

