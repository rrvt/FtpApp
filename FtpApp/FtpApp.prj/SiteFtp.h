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

#pragma once
#include "ftplib.h"
#include "Expandable.h"
#include "IterT.h"


class Site;


class SiteFtp;
typedef IterT<SiteFtp, String> DirsIter;


class SiteFtp {

FtpLib      ftp;
String      url;
bool        loggedIn;
String      rootPath;
String      lastRsp;
static bool logCmds;

Expandable<String, 2> dirs;               // A list of directories on the remote site

public:

  SiteFtp() : loggedIn(false) { }
 ~SiteFtp() { }

  bool    login(TCchar* urlName, TCchar* userID, TCchar* password);

  bool    isLoggedIn() {return loggedIn;}

  void    findNames(int n, Site& site);

  bool    size(   TCchar* webPath, int& size);
  bool    modDate(TCchar* webPath, Date& date);
  bool    getFile(TCchar* webPath,   TCchar* localPath);
  bool    putFile(TCchar* localPath, TCchar* webPath);
  bool    delFile(TCchar* webPath);

  String& lastResponse();

  void    close() {ftp.close();   loggedIn = false;}

  void    startLoggingData() {logCmds = true;}
  void    stopLoggingData()  {logCmds = false;}

  void    dpslyLastResponse(TCchar* title);
  void    dsplyLastError(TCchar* msg);

private:

  static bool certCT(X509* cert);

  void findSubDirs(TCchar* path);
  void updateFiles(TCchar* path, FtpStore& store, Site& site);

  static void log(Cchar* str, bool isCmd);

  // returns either a pointer to data (or datum) at index i in array or zero

  String* datum(int i) {return 0 <= i && i < nData() ? &dirs[i] : 0;}       // or data[i].p

  int   nData()      {return dirs.end();}                       // returns number of data items in array

  void  removeDatum(int i) {if (0 <= i && i < nData()) dirs.del(i);}

  friend typename DirsIter;
  };


extern SiteFtp siteFtp;
