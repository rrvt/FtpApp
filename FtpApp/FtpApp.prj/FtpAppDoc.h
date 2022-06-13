// FtpAppDoc.h : interface of the FtpAppDoc class


#pragma once
#include "CDoc.h"
#include "MainFrame.h"
#include "SiteID.h"

class SiteFile;


enum DataSource {NotePadSrc, StoreSrc, PrvSiteSrc, LclSiteSrc, UpdSiteSrc};


class FtpAppDoc : public CDoc {

DataSource  dataSource;

bool        cmdLock;
bool        savePrv;

protected: // create from serialization only

  FtpAppDoc() noexcept;
  DECLARE_DYNCREATE(FtpAppDoc)

public:

  virtual     ~FtpAppDoc();

    DataSource dataSrc() {return dataSource;}
          void display(DataSource ds);

          bool loadPrevSiteData();
          void saveSite(DataSource src);

  virtual void serialize(Archive& ar);


#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

private:

  String& dbPath(String& path) {return siteID.dbPath(path);}

  void    cmpPrvFile(SiteFile& uf);
  void    findDeleted();
  bool    put(SiteFile& uf);
  bool    get(SiteFile& uf);
  bool    del(SiteFile& uf);
  void    dspUpdates(bool onlyChkd = false);

  void    startWkrThrd(AFX_THREADPROC thdProc, uint arg, int n);
  bool    finWkrThrd(LPARAM lParam);

protected:

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onStartThread();

  afx_msg void OnNewSite();
  afx_msg void onEditSite();
  afx_msg void onPickSite();
          void loadSite();
          void finLoadSite(LPARAM lParam);    // finish Load Site
  afx_msg void OnUpdateSite();

  afx_msg void OnConfirmUpdate();
          void confirmUpdate();
          void cnfrmPrgs(LPARAM lParami);
          void finConfirm(LPARAM lParam);
//        void cnfrmStsErr(LPARAM lParam);

  afx_msg void onGetSite();                   // start thread to do the hard work and
          void finGetSite(LPARAM lParam);     // finish get site command
  afx_msg void OnOptions();
  afx_msg void onDeleteSite();
  };





//  void testLine(int n);
//  void wholePage();

// Generated message map functions
//  bool filterFile(SiteFile& sf);

