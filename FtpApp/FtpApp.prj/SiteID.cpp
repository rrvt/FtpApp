// Site ID Data


#include "stdafx.h"
#include "SiteID.h"
//#include "FtpApp.h"
#include "Ftp.h"
#include "IniFile.h"
#include "NotePad.h"
#include "SiteLoginDlg.h"
#include "SitePickerDlg.h"
#include "Utilities.h"


SiteID siteID;

       TCchar* GlobalSect   = _T("Global");
static TCchar* LastSiteKey  = _T("LastSite");
static TCchar* SiteNameKey  = _T("SiteName");                // IniFile key words
static TCchar* SitePathKey  = _T("Path");
static TCchar* URLNameKey   = _T("URLName");
static TCchar* UserIDKey    = _T("UserID");
static TCchar* PasswordKey  = _T("Password");
static TCchar* NoWebFlsKey  = _T("NoWebFiles");

//TCchar* LocalWebPath = _T("LocalWebPath");



void SiteID::clear() {
  name.clear(); root.clear(); url.clear(); userID.clear(); password.clear();

  noWebFiles = 0;   loggedIn = false;
  }



// Pick a site for a list of site the application knows and load it

bool SiteID::pickSite() {
SitePickerDlg dlg;
String        section;

  if (dlg.DoModal() != IDOK) return false;

  section = dlg.section;

  return load(section);
  }


bool SiteID::edit() {
SiteLoginDlg dlg;

  dlg.name = name;   dlg.rootTxt = root;   dlg.urlName = url;
  userID >> dlg.userID;   password >> dlg.password;

  if (dlg.DoModal() != IDOK) return false;

  name = dlg.name;   root = dlg.rootTxt;   url = dlg.urlName;
  userID << dlg.userID;   password << dlg.password;

  saveSiteData();  return true;
  }


bool SiteID::login() {
String user;   userID   >> user;    ToAnsi uid(user);
String pswd;   password >> pswd;    ToAnsi pwd(pswd);

  if (user.isEmpty() || pswd.isEmpty() || pswd.isEmpty()) return false;

  FtpLog(url);

  if (!ftp.open(url)) return false;

  loggedIn = ftp.login(user, pswd);   if (!loggedIn) return false;

  notePad << _T("Logged In") << nCrlf;

  ftp.pwd(webRoot);

  return true;
  }


bool SiteID::load(String& sectName) {
String user;
String pwd;

  if (sectName.isEmpty()) return false;

  iniFile.readString(sectName, SiteNameKey, name, _T(""));
  iniFile.readString(sectName, SitePathKey, root, _T(""));
  iniFile.readString(sectName, URLNameKey,  url,  _T(""));
  iniFile.readString(sectName, UserIDKey,   user, _T(""));    userID  = user;               //
  iniFile.readString(sectName, PasswordKey, pwd,  _T(""));   password = pwd;
  noWebFiles = iniFile.readInt(sectName, NoWebFlsKey, 50);

  fixRootPath(root);

  return true;
  }


void SiteID::saveSiteData() {         // "rrvtmg@swde.com", "1k1S*jxKJV!kkp&EI"
String sect;
String user;
String pwd;

  if (name.isEmpty()) return;

  sect = name + _T(" Site");

  iniFile.writeString(GlobalSect, LastSiteKey, sect);
  iniFile.writeString(sect,       SiteNameKey, name);
  iniFile.writeString(sect,       SitePathKey, root);
  iniFile.writeString(sect,       URLNameKey,  url);
  iniFile.writeString(sect,       UserIDKey ,  userID());
  iniFile.writeString(sect,       PasswordKey, password());
  iniFile.writeInt(   sect,       NoWebFlsKey, noWebFiles);
  }


bool SiteID::delCurSite() {
String sect;

  if (name.isEmpty()) return false;

  sect = name + _T(" Site");   iniFile.deleteSection(sect);   return true;
  }

