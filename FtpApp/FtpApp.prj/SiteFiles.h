// Site Files -- A list of all the web site files (filtered) in the local and remote site tree


#pragma once
#include "Date.h"
#include "ExpandableP.h"
#include "IterT.h"

class CSVLex;
class CSVOut;
class Archive;


enum   SiteFlSts {NilSts, PutSts, GetSts, DelSts};


class SiteFile {
public:

SiteFlSts status;
bool      check;

String    name;             // Name of file, <name>.<ext>
String    path;             // Relative to Root Path, key for Sort,
int       size;
Date      date;

  SiteFile() : status(NilSts), check(false), size(0), date(Date::MinDate) { }
 ~SiteFile() { }

  SiteFile(SiteFile& siteFile) {copy(siteFile);}                            // copy data
  SiteFile& operator= (SiteFile& siteFile) {copy(siteFile); return *this;}

  // Allows sorted data
  bool   operator>= (SiteFile& siteFile) {return _tcsicmp(path, siteFile.path) >= 0;}
  bool   operator== (SiteFile& siteFile) {return _tcsicmp(path, siteFile.path) == 0;}

  // Required for Binary Search
  bool operator== (TCchar* path) {return _tcsicmp(this->path, path) == 0;}
  bool operator<  (TCchar* path) {return _tcsicmp(this->path, path) <  0;}
  bool operator>  (TCchar* path) {return _tcsicmp(this->path, path) >  0;}

  void   addLclAttr(TCchar* path);

  bool   load(CSVLex& lex);
  void   save(CSVOut& csvOut);

  void   display();

private:

  void copy(SiteFile& siteFile) {
    status = siteFile.status;  check = siteFile.check;
    name   = siteFile.name;    path  = siteFile.path;
    size   = siteFile.size;    date  = siteFile.date;
    }
  };


class SiteFiles;

typedef DatumPtrT<SiteFile> SiteFileP;
typedef IterT<SiteFiles, SiteFile> FilesIter;         // Iterator Declaration


class SiteFiles {

String root;
int    rootLng;

HWND   hWnd;                // Data for posting progress messages
uint   msgID;

public:

ExpandableP<SiteFile, SiteFileP, 2> data;             // List of all files in web site (local and remote)

            SiteFiles() : rootLng(0) { }
           ~SiteFiles() {clear();}

  void      clear() {root.clear(); rootLng = 0; data.clear();}
  void      setHwnd(HWND h) {hWnd = h;}

  bool      isEmpty() {return nData() == 0;}

  void      startFromWeb(TCchar* path, HWND h, uint msgId);
  void      setRoot(TCchar* path) {root = path;   rootLng = root.length();}
  void      fromWeb(TCchar* path);
  void      fromPC(TCchar* path);
  bool      addFile(TCchar* path);

  void      update(SiteFile& uf);
  void      delRcd(SiteFile& uf);
  void      addFile(SiteFile& uf);

  bool      load(Archive& ar);
  bool      load(CSVLex& lex);
  void      save(Archive& ar);
  void      save(CSVOut& cvsOut);

  SiteFile* find(TCchar* path) {return data.bSearch(path);}

  void      display(TCchar* title);

  int       nData() {return data.end();}                 // returns number of data items in array
  SiteFile* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

private:

  bool filterFile(TCchar* path);

  // returns either a pointer to data (or datum) at index i in array or zero


  void      removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename FilesIter;
  };


extern SiteFiles prvFiles;
extern SiteFiles curFiles;
extern SiteFiles updateFiles;

