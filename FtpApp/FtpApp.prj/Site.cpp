// Site Description -- The name, login info, list of local directories and list of the local and remote
// files.


#include "stdafx.h"
#include "Site.h"
#include "Archive.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "FtpApp.h"
#include "SitePickerDlg.h"


static TCchar* SiteNameKey = _T("SiteName");                // IniFile key words
static TCchar* SitePathKey = _T("Path");
static TCchar* URLNameKey  = _T("URLName");
static TCchar* UserIDKey   = _T("UserID");
static TCchar* PasswordKey = _T("Password");


Site site;


// Reset all data for the site

void Site::clear() {
  siteName.clear(); rootPath.clear(); urlName.clear(); userID.clear(); password.clear();
  siteDirs.clear(); siteFiles.clear();
  }


// Pick a site for a list of site the application knows and load it

bool Site::pickSite() {
SitePickerDlg dlg;
String        section;

  if (dlg.DoModal() != IDOK) return false;

  section = dlg.section;

  return load(section);
  }



bool Site::initSiteData() {
String sectName;

  iniFile.readString(GlobalSect, LastSiteKey, sectName, _T(""));

  return load(sectName);
  }


bool Site::load(String& sectName) {

  if (sectName.isEmpty()) return false;

  iniFile.readString(sectName, SiteNameKey, siteName, _T(""));
  iniFile.readString(sectName, SitePathKey, rootPath, _T(""));
  iniFile.readString(sectName, URLNameKey,  urlName,  _T(""));
  iniFile.readString(sectName, UserIDKey ,  userID,   _T(""));
  iniFile.readPwd(   sectName, PasswordKey, password);

  return true;
  }


void Site::saveSiteData() {         // "rrvtmg@swde.com", "1k1S*jxKJV!kkp&EI"
String sect;

  if (siteName.isEmpty()) return;

  sect = siteName + _T(" Site");

  iniFile.writeString(GlobalSect, LastSiteKey, sect);
  iniFile.writeString(sect, SiteNameKey, siteName);
  iniFile.writeString(sect, SitePathKey, rootPath);
  iniFile.writeString(sect, URLNameKey,  urlName);
  iniFile.writeString(sect, UserIDKey ,  userID);
  iniFile.writePwd(   sect, PasswordKey, password);
  }


void Site::refresh() {
int n;

  siteDirs.clear();   siteFiles.clear();

  n = findNames(rootPath);   findWebNames(n);
  }


int Site::findNames(TCchar* path) {
DirIter  iter(siteDirs);
SiteDir* dir;
int      n;

  rootPath = path;

  siteDirs.findNames(rootPath);   n = siteDirs.nDirs();

  for (dir = iter(); dir; dir = iter++) siteFiles.findNames(dir->path);

  n += siteFiles.nFiles();   return n;
  }


void Site::display() {

  notePad << nClrTabs << nSetRTab(32) << nSetRTab(43) << nSetTab(45);

  notePad << _T("Web Site Name: ") << siteName << _T(" -- Path: ") << rootPath << nCrlf;

  notePad << nCrlf;

  siteDirs.display();  notePad << nCrlf;

  siteFiles.display();
  }


bool Site::load(Archive& ar) {
CSVLex     lex(ar);
CSVtokCode code;
CSVtokCode code1;
int        i;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); break;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : if (tok.name != siteName) return false;
                break;

      case 1  : if (tok.name != urlName) {
                  notePad << _T("Web Name differs: ") << urlName << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;
      case 2  : if (tok.name != rootPath) {
                  notePad << _T("Path differs: ") << rootPath << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  siteDirs.load(lex);   return siteFiles.load(lex);
  }



void Site::save(Archive& ar) {
CSVOut csvOut(ar);

  csvOut << siteName << Comma << urlName << Comma << rootPath << Comma << vCrlf;

  siteDirs.save(csvOut);
  siteFiles.save(csvOut);
  }



