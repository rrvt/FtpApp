

#include "stdafx.h"
#include "SiteFiles.h"
#include "Archive.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "Expandable.h"
#include "FileName.h"
#include "FileSrch.h"
#include "Ftp.h"
#include "FtpApp.h"
#include "FtpTransport.h"
#include "Resource.h"
#include "SiteID.h"
#include "Utilities.h"


SiteFiles prvFiles;
SiteFiles curFiles;
SiteFiles updateFiles;


void SiteFiles::startFromWeb(TCchar* path, HWND h, uint msgId)
                               {clear();   hWnd = h;   setRoot(path);   msgID = msgId;   fromWeb(root);}


void SiteFiles::fromWeb(TCchar* path) {
Expandable<String, 2> data;
int                   i;
int                   n;
FtpChIter             iter(transport);
int                   pos;
Tchar                 tch;
String                name;

  ftp.dir(path);

  for (String& s = *iter(); iter++; iter.accept()) data.nextData() = s;

  for (i = 0, n = data.end(); i < n; i++) {
    String& line = data[i];

    pos = line.findLastOf(_T(' '));  if (pos < 0) continue;
    name = line.substr(pos+1);   tch = name[0];   if (tch == _T('.') || tch == _T('_')) continue;

    if (line[0] == _T('d')) {String subDir = path + name + _T('/');   fromWeb(subDir); continue;}

    String fullPath = path + name;

    if (addFile(fullPath)) ::PostMessage(hWnd, msgID, ID_IncProgress, 0);
    }

  data.clear();
  }


void SiteFiles::fromPC(TCchar* path) {
Expandable<String, 2> data;
FileSrch              srch;
String                sub;
String                file;
int                   i;
int                   n;

  if (srch.findAllSubDirs(path)) while (srch.getName(sub)) data = sub;

  if (srch.findAllFiles(path)) while (srch.getName(file)) addFile(file);

  for (i = 0, n = data.end(); i < n; i++) fromPC(data[i]);

  data.clear();
  }


bool SiteFiles::addFile(TCchar* path) {
SiteFile* sf;
FileIO    lu;
CTime     time;

  if (filterFile(path)) return false;

  sf  = data.allocate();

  String&   pth = sf->path;   pth = path;

  if (pth.find(_T('/')) >= 0) {ftp.size(pth, sf->size);   ftp.date(pth, sf->date);}
  else                         sf->addLclAttr(path);

  pth      = localPath(pth.substr(rootLng));
  sf->name = removePath(pth);   SiteFile* p = data = sf;

  return true;
  }


void SiteFile::addLclAttr(TCchar* path) {
FileIO lu;
CTime  time;

  if (lu.open(path, FileIO::Read)) {

    size = lu.getLength();
    if (!lu.getModifiedTime(time)) time = CTime::GetCurrentTime();

    lu.close();
    }

  else {size = 1; time = CTime::GetCurrentTime();}

  date = time;
  }



void SiteFiles::update(SiteFile& uf) {
SiteFile* sf = find(uf.path);

  if (sf) {sf ->size = uf.size; sf->date = uf.date;}
  else     data = uf;
  }


void SiteFiles::addFile(SiteFile& uf) {SiteFile* sf = data.allocate();   *sf = uf;   data = sf;}



static TCchar* filters[] = {_T("_notes\\"), _T("Junk\\"),   _T(".log"),   _T("tmp\\"),  _T(".vpj"),
                            _T(".vpw"),     _T(".vpwhist"), _T(".vtg"),   _T(".xml")
                            };

bool SiteFiles::filterFile(TCchar* path) {
String s = localPath(path);
int    i;
int    n;

  for (i = 0, n = noElements(filters); i < n; i++ ) if (s.find(filters[i]) >= 0) return true;

  return false;
  }



void SiteFiles::delRcd(SiteFile& uf) {
FilesIter iter(*this);
SiteFile* sf;

  for (sf = iter(); sf; sf = iter++) if (sf->path == uf.path) {iter.remove(); return;}
  }



bool SiteFiles::load(Archive& ar) {
CSVLex     lex(ar);
CSVtokCode code;
CSVtokCode code1;
int        i;

  clear();

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); break;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code == CommaToken && code1 == StringToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : if (tok.name != siteID.name) return false;
                break;

      case 1  : if (tok.name != siteID.url) {
                  notePad << _T("Web Name differs: ") << siteID.url << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;
      case 2  : if (tok.name != siteID.root) {
                  notePad << _T("Path differs: ") << siteID.root << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  return load(lex);
  }


bool SiteFiles::load(CSVLex& lex) {
int i;

  data.clear();

  for (i = 0; ; i++) {
    SiteFile siteFile;

    if (!siteFile.load(lex)) return i > 0;

    data = siteFile;
    }
  }


bool SiteFile::load(CSVLex& lex) {
CSVtokCode code;
CSVtokCode code1;
int        i;
uint       x;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); return true;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : name = tok.name; break;

      case 1  : path = tok.name; break;

      case 2  : size = tok.name.stoi(x); break;

      case 3  : {ToDate lcldt(tok.name);  date = lcldt();} break;

      case 4  : size = tok.name.stoi(x); break;

      case 5  : {ToDate rmtdt(tok.name);  date = rmtdt();} break;

//    case 6  : remotePath = tok.name; break;

      default : notePad << _T("Whoops!") << nCrlf;
      }
    lex.accept_two_tokens();
    }

  return false;
  }



void SiteFiles::save(Archive& ar) {
CSVOut csvOut(ar);

  csvOut << siteID.name << Comma << siteID.url << Comma << siteID.root << Comma << vCrlf;

  save(csvOut);
  }


void SiteFiles::save(CSVOut& csvOut) {
FilesIter iter(*this);
SiteFile* siteFile;

  for (siteFile = iter(); siteFile; siteFile = iter++) {
    siteFile->save(csvOut);
    }
  }


void SiteFile::save(CSVOut& csvOut) {

  csvOut << name << Comma;
  csvOut << path << Comma;
  csvOut << size << Comma;
  csvOut << date << Comma;
  csvOut << vCrlf;
  }


void SiteFiles::display(TCchar* title) {
FilesIter iter(*this);
SiteFile* fl;

  notePad << nClrTabs << nSetRTab(32) << nSetRTab(43) << nSetTab(45) << nCrlf;

  notePad << nBeginLine << title << _T(" Files -- Path: ") << root;
  notePad << nTab << nData() << nEndLine << nCrlf << nCrlf;

  for (fl = iter(); fl; fl = iter++) fl->display();
  }


void SiteFile::display() {
String d  = date;
String rd = date;

  notePad << name << nTab << size << nTab << d << nTab << path << nCrlf;
  }




#if 0
bool Site::loadLclNames(TCchar* path) {
DirIter  iter(siteDirs);
SiteDir* dir;
int      rootLng;
int      n;

  rootPath = path;   rootLng = rootPath.length();

  siteDirs.loadLclNames(rootPath);   n = siteDirs.nData();

  for (dir = iter(); dir; dir = iter++) siteFiles.loadLclNames(dir->path, rootLng);

  n += siteFiles.nData();   return n > 0;
  }


// Find all sub-directory names and place them in the list

void SiteDirs::loadLclNames(TCchar* fullPath) {
String    path = fullPath;
FileSrch  srch;
String    sub;
DirIter   iter(*this);
SiteDir   siteDir;

  if (path.isEmpty()) return;

  siteDir.set(path);

  if (filter(siteDir.name)) return;

  data = siteDir;                       // Place in sorted list

  if (srch.findAllSubDirs(fullPath)) while (srch.getName(sub)) loadLclNames(sub);
  }


void SiteDirs::add(TCchar* path) {
SiteDir* siteDir = data.allocate();

  siteDir->path = localPath(path);   siteDir->name = getDirName(siteDir->path);   data += siteDir;
  }


// Using the directory list find all files in each sub-directory

bool SiteFiles::loadLclNames(TCchar* pth, int rootLng) {
FileSrch srch;
String   path;

  if (!srch.findAllFiles(pth)) return false;

  while (srch.getName(path)) {
    SiteFile siteFile;   if (siteFile.set(path, rootLng)) data = siteFile;
    }

  return true;
  }
#endif
#if 0
bool SiteFile::set(TCchar* pth, int rootLng) {
FileIO lu;
CTime  time;
String fullPath;

  path = fullPath = pth;   path = path.substr(rootLng);

  name = parseName(path);

  if (filter()) return false;

  if (lu.open(fullPath, FileIO::Read)) {
    size = lu.getLength();

     CTime tm = lu.getModifiedTime(time) ? time : CTime::GetCurrentTime();   date = tm;

    lu.close();
    }

  return true;
  }
#endif
#if 0
  {
    if (lu.open(pth, FileIO::Read)) {

      sf->size = lu.getLength();
      if (!lu.getModifiedTime(time)) time = CTime::GetCurrentTime();

      lu.close();
      }
    else {sf->size = 1; time = CTime::GetCurrentTime();}

    sf->date = time;
    }
#endif

