// Site Update Dialog


#include "stdafx.h"
#include "SiteUpdateDlg.h"
#include "Utilities.h"


// SiteUpdateDlg dialog

IMPLEMENT_DYNAMIC(SiteUpdateDlg, CDialogEx)


SiteUpdateDlg::SiteUpdateDlg(SiteFiles& updateFiles, CWnd* pParent) :
                                              CDialogEx(IDD_SiteUpdate, pParent), files(updateFiles) { }

SiteUpdateDlg::~SiteUpdateDlg() { }


BOOL SiteUpdateDlg::OnInitDialog() {
FilesIter iter(files);
SiteFile* siteFile;
String    s;
int       index;

  CDialogEx::OnInitDialog();

  for (siteFile = iter(); siteFile; siteFile = iter++) {
    switch (siteFile->status) {
      case SiteFile::Delete : s = _T(" Delete File: "); break;
      case SiteFile::Changed: s = _T(" Update File: "); break;
      case SiteFile::PutFile: s = _T(" Put File:    "); break;
      case SiteFile::GetFile: s = _T(" Get File:    "); break;
      default               : s = _T(" Unknown:     "); break;
      }

    s += remotePath(siteFile->path);

    index = listCtrl.AddString(s);   listCtrl.SetItemData(index, (DWORD_PTR) siteFile);
    }

  return TRUE;
  }


void SiteUpdateDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_UpdateList, listCtrl);
  }


BEGIN_MESSAGE_MAP(SiteUpdateDlg, CDialogEx)
  ON_BN_CLICKED(IDC_UpdateAll, &SiteUpdateDlg::OnUpdateAll)
END_MESSAGE_MAP()


// SiteUpdateDlg message handlers


void SiteUpdateDlg::OnOK() {
int       n = listCtrl.GetCount();
int       i;
SiteFile* file;

  for (i = 0; i < n; i++) {
    Cstring cs;

    listCtrl.GetText(i, cs);

    file = (SiteFile*) listCtrl.GetItemData(i);

    if (file) file->update = listCtrl.GetCheck(i) == BST_CHECKED;
    }

  CDialogEx::OnOK();
  }


void SiteUpdateDlg::OnUpdateAll() {
int       n = listCtrl.GetCount();
int       i;
SiteFile* file;

  for (i = 0; i < n; i++) {
    Cstring cs;   listCtrl.GetText(i, cs);

    file = (SiteFile*) listCtrl.GetItemData(i);

    if (file) file->update = true;
    }

  CDialogEx::OnOK();
  }
