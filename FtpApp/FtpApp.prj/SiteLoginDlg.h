// Web Login Dialog Box -- Allows the user to supply the information needed to login to the web host

#pragma once


class SiteLoginDlg : public CDialogEx {

  DECLARE_DYNAMIC(SiteLoginDlg)

public:
CString name;
CButton LclRootBtn;
CStatic LclRootTxt;
CString rootTxt;
CString urlName;
CString userID;
CString password;

           SiteLoginDlg(CWnd* pParent = nullptr);              // standard constructor
  virtual ~SiteLoginDlg();

  virtual BOOL OnInitDialog();
  virtual void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_WebLogin };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);            // DDX/DDV support

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void OnClickedLclrootbtn();
  };
