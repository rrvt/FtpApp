// Web Login Dialog Box -- Allows the user to supply the information needed to login to the web host

#pragma once


class WebLoginDlg : public CDialogEx {

  DECLARE_DYNAMIC(WebLoginDlg)

public:
CString name;
CString urlName;
CString userID;
CString password;

           WebLoginDlg(CWnd* pParent = nullptr);              // standard constructor
  virtual ~WebLoginDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_WebLogin };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);            // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
};
