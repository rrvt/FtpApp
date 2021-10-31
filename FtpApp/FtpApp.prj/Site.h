// Site Description -- The name, login info, list of local directories and list of the local and remote
// files.
//
// This file also declares the "old" or "former" site descriptor.


#pragma once
#include "NotePad.h"
#include "SiteDirs.h"
#include "SiteFiles.h"
#include "SiteFtp.h"
#include "Utilities.h"


class Archive;


class Site {
public:

String    siteName;                     // Overall name for Site
String    rootPath;                     // Path to local web site files
String    urlName;                      // name to use in ftp login
String    userID;                       // user ID for ftp login
String    password;                     // password for ftp login

SiteDirs  siteDirs;                     // List of all directories in the site
SiteFiles siteFiles;                    // List of all web site files in the local and remote OSes

  Site() { }
 ~Site() { }

  void      clear();

  bool      pickSite();

  bool      login() {return siteFtp.login(urlName, userID, password);}

  bool      initSiteData();
  bool      load(Archive& ar);

  void      add(SiteFile& file) {siteFiles.add(file);}
  void      addWebData(TCchar* path, int size, Date& date) {siteFiles.addWebData(path, size, date);}
  void      setWebAttr(TCchar* path) {SiteFile* file = find(path);  if (file) file->setWebAttr();}
  void      setLclAttr(TCchar* path) {SiteFile* file = find(path);  if (file) file->setLclAttr();}
  void      delRcd(TCchar* path) {SiteFile* file = find(path);  siteFiles.del(file);}

  void      display();
  void      dspUpdates(SiteFiles& updateFiles) {updateFiles.dspUpdates();}

  void      refresh();

  int       findNames(TCchar* path);

  void      findWebNames(int n) {siteFtp.findNames(n, *this);}
  bool      getFile(TCchar* path)
                              {String s = rootPath + path;  return siteFtp.getFile(remotePath(path), s);}
  bool      putFile(TCchar* path)
                              {String s = rootPath + path;  return siteFtp.putFile(s, remotePath(path));}
  bool      delFile(TCchar* path) {return siteFtp.delFile(remotePath(path));}

  SiteFile* find(TCchar* path) {return siteFiles.find(path);}

  void      saveSiteData();
  void      save(Archive& ar);

private:

  bool      load(String& sectName);

  };


extern Site site;           // The "old" copy of site for a web site.

