// In the MFC doc/view organization most of the commands are performed here (or they launch other objects
// to do the actual work)

#include "stdafx.h"
#include "FtpAppDoc.h"
#include "filename.h"
#include "FtpApp.h"
#include "FtpAppView.h"
#include "Options.h"
#include "Resource.h"
#include "Site.h"
#include "SiteUpdateDlg.h"
#include "WebLoginDlg.h"


// FtpAppDoc

IMPLEMENT_DYNCREATE(FtpAppDoc, CDoc)

BEGIN_MESSAGE_MAP(FtpAppDoc, CDoc)

  ON_COMMAND(ID_NewSite,        &OnNewSite)
  ON_COMMAND(ID_LoginSite,      &OnLoginSite)
  ON_COMMAND(ID_UpdateSite,     &OnUpdateSite)
  ON_COMMAND(ID_ConfirmUpdate,  &OnConfirmUpdate)
  ON_COMMAND(ID_Refresh,        &OnRefresh)
  ON_COMMAND(ID_DeleteSite,     &OnDeleteSite)

  ON_COMMAND(ID_SaveSite,       &OnSaveSite)
  ON_COMMAND(ID_Options,        &OnOptions)

END_MESSAGE_MAP()


// FtpAppDoc construction/destruction

FtpAppDoc::FtpAppDoc() noexcept : dataSource(NotePadSrc) { }

FtpAppDoc::~FtpAppDoc() { }



// Create a new web site database and save it

void FtpAppDoc::OnNewSite() {
int n;

  notePad.clear();   site.clear();   siteFtp.close();

  iniFile.readString(GlobalSect, LocalWebPath, path);

  getDirPathDlg(_T("Web Site"), path);    site.rootPath = path;

  iniFile.writeString(GlobalSect, LocalWebPath, ::getPath(path));

  if (!login()) return;

  n = site.findNames(path);   site.findWebNames(n);   OnSaveSite();

  site.display();   display(NotePadSrc);
  }


bool FtpAppDoc::login() {
WebLoginDlg dlg;

  if (!site.siteName.isEmpty()) {
    dlg.name = site.siteName;   dlg.urlName = site.urlName;   dlg.userID = site.userID;
    dlg.password = site.password;
    }

  if (dlg.DoModal() != IDOK) return false;

  site.siteName = dlg.name;   site.urlName = dlg.urlName;   site.userID = dlg.userID;
  site.password = dlg.password;

  site.saveSiteData();

  if (!siteFtp.login(dlg.urlName, dlg.userID, dlg.password)) return false;

  return true;
  }


// Pick new site to manipulate from all the sites this application knows

void FtpAppDoc::OnLoginSite() {

  notePad.clear();   site.clear();   siteFtp.close();

  if (!site.pickSite()) return;

  if (loadSiteData()) {site.login();   site.findNames(site.rootPath);}

  site.display();   display(NotePadSrc);
  }


// Just refresh the list of files (just in case one is nervous)

void FtpAppDoc::OnRefresh() {

  notePad.clear();

  notePad << nBeginLine << _T("Refreshing Site Database") << nEndLine << nCrlf << nCrlf;

  if (!login()) return;

  site.refresh();   site.display();   display(NotePadSrc);
  }


// Delete the database related to the current site, NO WEB SITE FILES ARE DELETED!

void FtpAppDoc::OnDeleteSite() {
String siteName = site.siteName;
String path = theApp.roamingPath() + siteName + _T(".csv");
String sect = siteName + _T(" Site");

  notePad.clear();   notePad << _T("Deleting ") << sect << nCrlf;

  removeFile(path);

  iniFile.deleteString(GlobalSect, LastSiteKey);

  iniFile.deleteSection(sect);

  site.clear();    display(NotePadSrc);
  }


// Start the update process by finding all the local and remote web site files and doing some
// comparisons of the "current" list and the "former" or "older" list.

void FtpAppDoc::OnUpdateSite() {
Site       curSite;
FilesIter  iter(curSite.siteFiles);
SiteFile*  curFl;
SiteFile*  oldFl;
FilesIter  siteIter(site.siteFiles);
int        n;

  notePad.clear();   updateFiles.clear();   site.login();

  n = curSite.findNames(site.rootPath);
  curSite.findWebNames(n);

  for (curFl = iter(); curFl; curFl = iter++) {

    oldFl = site.find(curFl->path);

    if (!oldFl) {add(*curFl, SiteFile::PutFile);   continue;}          // Add later

    oldFl->webPresent = true;

    if (curFl->localSize == 0) {

      if (curFl->remoteSize != 0) {add(*curFl, SiteFile::GetFile); continue;}

      if (oldFl->localSize  != 0) {add(*curFl, SiteFile::Delete);  continue;}

      add(*curFl, SiteFile::Delete);  notePad << _T("local size == 0 and not processed!") << nCrlf;

      continue;
      }

    if (curFl->localSize  != oldFl->localSize)           {add(*curFl, SiteFile::Changed); continue;}

    if (curFl->localDate  != oldFl->localDate)           {add(*curFl, SiteFile::Changed); continue;}

    if (curFl->remoteSize != oldFl->remoteSize)          {add(*curFl, SiteFile::Changed); continue;}

    if (curFl->remoteSize == 0) {add(*curFl, SiteFile::PutFile); continue;}
    }

  for (curFl = siteIter(); curFl; curFl = siteIter++)
    if (curFl->status == SiteFile::NilStatus && !curFl->webPresent) add(*curFl, SiteFile::Delete);

  site.dspUpdates(updateFiles);   display(NotePadSrc);
  }


void FtpAppDoc::add(SiteFile& siteFile, SiteFile::Status status)
                                                 {siteFile.status = status;   updateFiles.add(siteFile);}


// Present the list of files for which some operation is indicated by the "Update" function above.
// Allow the user to select those updates to perform (or select do all updates).  Then perform
// the operations, update the database (list of files, etc) and save the database.

void FtpAppDoc::OnConfirmUpdate() {
SiteUpdateDlg dlg(updateFiles);
FilesIter     iter(updateFiles);
SiteFile*     file;
bool          saveFiles = false;

  if (dlg.DoModal() == IDOK) {
    site.dspUpdates(updateFiles);   display(NotePadSrc);

    for (file = iter(); file; file = iter++) {

      if (!file->update) continue;

      switch (file->status) {
        case SiteFile::Delete : file->delFile();
                                site.delRcd(file->path);  saveFiles = true;   break;

        case SiteFile::GetFile: file->getFile();   site.add(*file);
                                site.setLclAttr(file->path); saveFiles = true;  break;

        case SiteFile::Changed:
        case SiteFile::PutFile: file->putFile();
                                site.setWebAttr(file->path);  site.setLclAttr(file->path);
                                saveFiles = true;  break;

        default               : notePad << _T("Unknown Operation:  ") << file->path << nCrlf;
        }
      }

    if (saveFiles) OnSaveSite();
    }
  }


// Manipulate the display and printer options

void FtpAppDoc::OnOptions() {options(view());  view()->setOrientation(options.orient);}



void FtpAppDoc::display(DataSource ds) {dataSource = ds; invalidate();}


// Load the "current site" into the application.  The database consists of a csv file located in the
// "Roaming/FtpApp/" directory.

void FtpAppDoc::OnLoadSite() {

  notePad.clear();   loadSiteData();   site.display();   display(NotePadSrc);
  }


// The load and save functions do the necessary things to open files and then call the serialize
// function which switches to the correct operation and function to do the work.  This was the MFC
// way of doing business and I just copied with my own flavor of opening files, etc...

bool FtpAppDoc::loadSiteData() {
String path = theApp.roamingPath() + site.siteName + _T(".csv");

  dataSource = SiteSrc;

  if (!OnOpenDocument(path)) return false;

  dataSource = NotePadSrc;  return true;
  }


void FtpAppDoc::OnSaveSite() {
String path = theApp.roamingPath() + site.siteName + _T(".csv");

  dataSource = SiteSrc;

  onSaveDocument(path);

  display(NotePadSrc);
  }


// UglyDoc serialization

void FtpAppDoc::serialize(Archive& ar) {

  if (ar.isStoring())
    switch(dataSource) {
      case NotePadSrc : notePad.archive(ar); return;
      case SiteSrc    : site.save(ar); return;
      default         : return;
      }

  else
    switch(dataSource) {
      case SiteSrc    : site.load(ar);   return;
      default         : return;
      }
  }


// FtpAppDoc diagnostics

#ifdef _DEBUG

void FtpAppDoc::AssertValid() const {CDocument::AssertValid();}

void FtpAppDoc::Dump(CDumpContext& dc) const {CDocument::Dump(dc);}

#endif //_DEBUG


