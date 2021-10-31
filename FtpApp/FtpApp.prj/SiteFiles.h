// Site Files -- A list of all the web site files (filtered) in the local and remote site tree


#pragma once
#include "Date.h"
#include "ExpandableP.h"
#include "IterT.h"


class CSVLex;
class CSVOut;


class SiteFile {
public:

String name;                          // File Name, e.g. index.htm
String path;                          // Local Path of the file starting at the root of the web site
                                      // tree, e.g. if the local path to swde is "D:\Web\swde\index.htm"
                                      // then site.rootPath is "D:\Web\swde" and path would be
                                      // "\index.htm".
                                      // Note:  using the Windows separator '\' here
int    localSize;
Date   localDate;

int    remoteSize;
Date   remoteDate;

enum   Status {NilStatus, PutFile, GetFile, Changed, Delete};
Status status;
bool   webPresent;
bool   update;

  SiteFile() : localSize(0), localDate(Date::MinDate), remoteSize(0), remoteDate(Date::MinDate),
                                                  status(NilStatus), webPresent(false), update(false) { }
 ~SiteFile() { }

  SiteFile(SiteFile& siteFile) {copy(siteFile);}                            // copy data
  SiteFile& operator= (SiteFile& siteFile) {copy(siteFile); return *this;}

  bool   operator>= (SiteFile& siteFile) {return path >= siteFile.path;}  // Allows sorted data
  bool   operator== (SiteFile& siteFile) {return path == siteFile.path;}

  bool   set(TCchar* pth);
  void   setWebAttr();
  void   setLclAttr();
  bool   getFile();
  bool   putFile();

  bool   delFile();

  void   display();
  void   dspUpdate();

  bool   load(CSVLex& lex);
  void   save(CSVOut& csvOut);

private:

  bool filter();

  void setRemotePath(String& path);

  void copy(SiteFile& siteFile) {
    name       = siteFile.name;              path = siteFile.path;
    localSize  = siteFile.localSize;    localDate = siteFile.localDate;
    remoteSize = siteFile.remoteSize;  remoteDate = siteFile.remoteDate;
    status     = siteFile.status;      webPresent = siteFile.webPresent;
    update     = siteFile.update;
    }
  };



class SiteFiles;

typedef RcdPtrT<SiteFile> SiteFileP;
typedef IterT<SiteFiles, SiteFile> FilesIter;         // Iterator Declaration


class SiteFiles {
public:

ExpandableP<SiteFile, SiteFileP, 2> data;             // List of all files in web site (local and remote)

             SiteFiles() { }
            ~SiteFiles() { }

  void       clear() {data.clear();}

  bool       isEmpty() {return nData() == 0;}

  bool       findNames(TCchar* pth);
  int        nFiles() {return data.end();}

  void       addWebData(TCchar* path, int size, Date& date);

  void       refresh(TCchar* pth);

  SiteFile*  find(TCchar* localPath);

  void       display();
  void       dspUpdates();

  bool       load(CSVLex& lex);
  void       add(SiteFile& file) {data = file;}

  void       del(SiteFile* file) {data.del(file);}

  void       save(CSVOut& cvsOut);

private:

  // returns either a pointer to data (or datum) at index i in array or zero

  SiteFile* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  int   nData()      {return data.end();}                      // returns number of data items in array

  void  removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename FilesIter;
  };



