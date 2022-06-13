// Site ID Data


#pragma once
#include "FtpApp.h"
#include "Utilities.h"


extern TCchar* GlobalSect;


class SiteID {
public:

String      name;                           // Overall name for Site
String      root;                           // Local Root Path
String      webRoot;                        // Web Root Path
String      url;                            // name to use in ftp login
Baffle      userID;                         // user ID for ftp login
Baffle      password;                       // password for ftp login
int         noWebFiles;                     // ~Number of files on Web Host
bool        loggedIn;


  SiteID() : noWebFiles(0), loggedIn(false) { }
 ~SiteID() { }

  void clear();

  bool pickSite();

  bool edit();

  bool login();

  bool load(String& sectName);
  void saveSiteData();

  String& dbPath(String& path) {path = theApp.roamingPath() + name + _T(".csv");   return path;}

  bool delCurSite();
  };


extern SiteID siteID;
