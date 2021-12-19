// Site Directory -- Note only contains the directories on the local version of the web site


#pragma once
#include "ExpandableP.h"
#include "IterT.h"


class CSVOut;
class CSVLex;


class SiteDir {
public:
String name;
String path;

  SiteDir() { }
 ~SiteDir() { }

  SiteDir(SiteDir& datum) {copy(datum);}                            // required to expand data (below)
  SiteDir& operator= (SiteDir& datum) {copy(datum); return *this;}

  void set(TCchar* relPath);

  bool load(CSVLex& lex);
  void save(CSVOut& csvOut);

  bool operator >= (SiteDir& r) {return path >= r.path;}        // Allows data to be sorted
  bool operator == (SiteDir& r) {return path == r.path;}

private:

  void copy(SiteDir& datum) {name = datum.name; path = datum.path;}
  };


class SiteDirs;

typedef DatumPtrT<SiteDir> SiteDirP;
typedef IterT<SiteDirs, SiteDir> DirIter;                       // Iterator declaration


class SiteDirs {

ExpandableP<SiteDir, SiteDirP, 2> data;

public:

            SiteDirs() { }
           ~SiteDirs() { }

  void      clear() {data.clear();}

  void      findNames(TCchar* fullPath);
  int       nDirs() {return data.end();}

  void      display();

  bool      load(CSVLex& lex);
  void      save(CSVOut& csvOut);

private:

  bool filter(TCchar* name);

  // returns either a pointer to data (or datum) at index i in array or zero

  SiteDir* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  int   nData()      {return data.end();}                     // returns number of data items in array

  void  removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename DirIter;
  };

