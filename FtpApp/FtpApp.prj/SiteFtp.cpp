// Site Ftp Interface -- Only one web host, this is the interface to it...

// Notes:
//   * The romote site is Unix Based so the path separator is '/'
//   * The remote site starts at the "root" directory '/'
//   * Connecting and Logining in are facilitated by OpenSSL
//   * The Ftp Commands are not encrypted
//   * The ftp operations need 8 bit (Ansi) characters while the application wallows in Unicode (16 bit)
//     characters.  This module deals with the translations.
//   * Additionally, the module deals with the Unix style directory information to capture the
//     sub-directories and files on the host and provide size and date information to the file list.


#include "stdafx.h"
#include "SiteFtp.h"
#include "FtpApp.h"
#include "FtpCmdLog.h"
#include "FtpStore.h"
#include "NotePad.h"
#include "ProgBar.h"
#include "Site.h"


SiteFtp siteFtp;                            // Only one host and this is the interface object
bool    SiteFtp::logCmds = false;


bool SiteFtp::login(TCchar* urlName, TCchar* userID, TCchar* password) {
String host;
ToAnsi uid(userID);
ToAnsi pwd(password);

  if (loggedIn) {if (url == urlName) return true;   close();}

  ftpCmdLog.setCB(log);
//ftp.setCertCB(certCT);                // Uncomment if you want to do something with the certificate

  if (!urlName  || !_tcslen(urlName))  return false;
  if (!userID   || !_tcslen(userID))   return false;
  if (!password || !_tcslen(password)) return false;

  url = urlName;

  host = _T("ftp.");  host += urlName;   host += _T(":21");
  ToAnsi hostAnsi(host);

  if (!ftp.connect(hostAnsi())) {progBar.stopTimer(); return false;}

  loggedIn = ftp.login(uid(), pwd());

  if (loggedIn) ftp.pwd(rootPath);

  return loggedIn;
  }


// A function that would get a pointer to a certificate

bool SiteFtp::certCT(X509* cert) {
  return true;
  }


void SiteFtp::findNames(int n, Site& site) {
DirsIter iter(*this);
String*  dir;
FtpStore store;

  dirs.clear();

  progBar.open(n + 2);    progBar.step();

  findSubDirs(rootPath);

  for (dir = iter(); dir; dir = iter++) {

    ToAnsi ansi(*dir);   ftp.dir(ansi(), store);

    updateFiles(*dir, store, site);
    }

  progBar.close();
  }


void SiteFtp::updateFiles(TCchar* path, FtpStore& store, Site& site) {
FtpStoreIter iter(store);
String*      file;
String       name;
Tchar        tch;
int          pos;
String       fullPath;
int          size;
Date         date;

  for (file = iter(); file; file = iter++) {
    String& line = *file;

    if (line[0] == _T('d')) continue;

    pos = line.findLastOf(_T(' '));  if (pos < 0) continue;

    name = line.substr(pos+1);   tch = name[0];

    if (tch == _T('.') || tch == _T('_')) continue;

    fullPath = path + name;   ToAnsi ansi(fullPath);

    ftp.size(ansi(),    size);
    ftp.modDate(ansi(), date);

    site.addWebData(fullPath, size, date);

    progBar.step();
    }
  }



void SiteFtp::findSubDirs(TCchar* path) {
String       subDir = path;
ToAnsi       ansi(path);
FtpStore     ftpStore;
FtpStoreIter iter(ftpStore);
String*      dir;
int          pos;
String       name;
Tchar        tch;

  dirs += subDir;

  ftp.dir(ansi(), ftpStore);

  for (dir = iter(); dir; dir = iter++) {

    if ((*dir)[0] != _T('d')) continue;

    pos = dir->findLastOf(_T(' '));  if (pos < 0) continue;

    name = (*dir).substr(pos+1);   tch = name[0];   if (tch == _T('.') || tch == _T('_')) continue;

    progBar.step();

    subDir = path + name + _T('/');   findSubDirs(subDir);
    }
  }



bool SiteFtp::size(TCchar* webPath, int& size) {
  if (!loggedIn) return false;

  ToAnsi ansi(webPath);   return ftp.size(ansi(), size);
  }


bool SiteFtp::modDate(TCchar* webPath, Date& date) {

  if (!loggedIn) return false;

  ToAnsi ansi(webPath);   return ftp.modDate(ansi(), date);
  }


bool SiteFtp::getFile(TCchar* webPath,   TCchar* localPath) {
FtpStore ftpStore;

  if (!loggedIn) return false;

  ToAnsi ansi(webPath);   return ftp.get(ansi(), localPath, ftpStore);
  }


bool SiteFtp::putFile(TCchar* localPath, TCchar* webPath) {
FtpStore ftpStore;

  if (!loggedIn) return false;

  ToAnsi ansi(webPath);   return ftp.put(localPath, ansi(), ftpStore);
  }


bool SiteFtp::delFile(TCchar* webPath) {

  if (!loggedIn) return false;

  ToAnsi ansi(webPath);   return ftp.del(ansi());
  }



String& SiteFtp::lastResponse() {return lastRsp = ftp.lastResponse();}


void SiteFtp::log(Cchar* str, bool isCmd) {

  if (!logCmds) return;

ToUniCode uni(str);
String    s = uni();    s.trim();

  if (isCmd) notePad << _T("Cmd: ");
  else       notePad << _T("Rsp: ");

  notePad << s << nCrlf; return;
  }





void SiteFtp::dpslyLastResponse(TCchar* title)
                                              {notePad << nTab << title << ftp.lastResponse() << nCrlf;}


void SiteFtp::dsplyLastError(TCchar* msg) {
#if 1
#else

String s = ftpGetError();

  s = s.isEmpty() ? msg : msg + _T(' ') + s;

  messageBox(s);
#endif
  }


void logEx(Cchar* str, bool isCmd) {
ToUniCode uni(str);
String    s = uni();    s.trim();

  if (isCmd) notePad << _T("Cmd: ");
  else       notePad << _T("Rsp: ");

  notePad << s << nCrlf; return;
  }




