// Web Login Dialog Box


#include "stdafx.h"
#include "WebLoginDlg.h"
#include "Resource.h"


IMPLEMENT_DYNAMIC(WebLoginDlg, CDialogEx)

WebLoginDlg::WebLoginDlg(CWnd* pParent) : CDialogEx(IDD_WebLogin, pParent), urlName(_T("")),
                                                                      userID(_T("")), password(_T("")) { }

WebLoginDlg::~WebLoginDlg() { }


void WebLoginDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_Name,     name);
  DDX_Text(pDX, IDC_URLName,  urlName);
  DDX_Text(pDX, IDC_UserID,   userID);
  DDX_Text(pDX, IDC_Password, password);
  }


BEGIN_MESSAGE_MAP(WebLoginDlg, CDialogEx)
END_MESSAGE_MAP()
