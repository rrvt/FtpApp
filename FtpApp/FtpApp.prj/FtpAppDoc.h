// FtpAppDoc.h : interface of the FtpAppDoc class


#pragma once
#include "CDoc.h"
#include "MainFrame.h"
#include "SiteFiles.h"
#include "TBComboBox.h"
#include "TBEditBox.h"


enum DataSource {NotePadSrc, SiteSrc, NewSiteSrc};          // StoreSrc,


class FtpAppDoc : public CDoc {

PathDlgDsc  pathDlgDsc;

DataSource  dataSource;

SiteFiles   updateFiles;

protected: // create from serialization only

  FtpAppDoc() noexcept;
  DECLARE_DYNCREATE(FtpAppDoc)

public:

      DataSource dataSrc() {return dataSource;}

          bool   login();

          void   display(DataSource ds);

  virtual void   serialize(Archive& ar);

// Implementation
public:

  virtual ~FtpAppDoc();

  bool loadSiteData();

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

private:

  void add(SiteFile& siteFile, SiteFile::Status status);

  void testLine(int n);
  void wholePage();
  void compareFiles();

// Generated message map functions

protected:

  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnNewSite();
  afx_msg void OnLoginSite();
  afx_msg void OnUpdateSite();
  afx_msg void OnConfirmUpdate();
  afx_msg void OnRefresh();
  afx_msg void OnLoadSite();
  afx_msg void OnSaveSite();
  afx_msg void OnDeleteSite();
  afx_msg void OnOptions();
  };








#ifdef Examples
  afx_msg void OnTest();
  afx_msg void displayDataStore();
  afx_msg void myButton();

  afx_msg void OnComboBoxChng();

  afx_msg void OnTBEditBox();
  afx_msg void myButton1();
  afx_msg void onOption1();
  afx_msg void onOption2();
  afx_msg void onOption3();
  afx_msg void onOption21();
  afx_msg void onOption22();
  afx_msg void onOption23();
  afx_msg void OnTestEditBoxes();
#endif

