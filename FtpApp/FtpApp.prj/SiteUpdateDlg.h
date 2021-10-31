// Site Update Dialog -- Presents the files that the user may consider for updating


#pragma once
#include "Resource.h"
#include "SiteFiles.h"



// SiteUpdateDlg dialog

class SiteUpdateDlg : public CDialogEx {

  DECLARE_DYNAMIC(SiteUpdateDlg)

SiteFiles& files;

public:
CCheckListBox listCtrl;

               SiteUpdateDlg(SiteFiles& updateFiles, CWnd* pParent = nullptr);   // standard constructor
  virtual     ~SiteUpdateDlg();

  virtual BOOL OnInitDialog();
  virtual void OnOK();

  afx_msg void OnUpdateAll();

// Dialog Data

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_SiteUpdate };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
  };
