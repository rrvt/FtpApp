// Site Files -- A list of all the web site files (filtered) in the local and remote site tree


#include "stdafx.h"
#include "SiteFiles.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "filesrch.h"
#include "NotePad.h"
#include "Site.h"
#include "SiteFtp.h"
#include "Utilities.h"


// Using the directory list find all files in each sub-directory

bool SiteFiles::findNames(TCchar* pth) {
FileSrch srch;
String   path;

  if (!srch.findAllFiles(pth)) return false;

  while (srch.getName(path)) {
    SiteFile siteFile;   if (siteFile.set(path)) data = siteFile;
    }

  return true;
  }


// Called form the SiteFtp module to add the web host size and date to the file's record

void SiteFiles::addWebData(TCchar* path, int size, Date& date) {
String    lclPath = localPath(path);
SiteFile* siteFile;

  siteFile = find(lclPath);

  if (!siteFile) {
    SiteFile file;

    file.path       = lclPath;      file.name = parseName(lclPath);
    file.remoteSize = size;   file.remoteDate = date;

    add(file);   return;
    }

  siteFile->remoteSize = size;   siteFile->remoteDate = date;
  }



// The SiteFile object has all the info it needs to determine its size and date

bool SiteFile::set(TCchar* pth) {
FileIO lu;
CTime  time;
String fullPath;

  path = fullPath = pth;   path = path.substr(site.rootPath.length());

  name = parseName(path);

  if (filter()) return false;

  if (lu.open(fullPath, FileIO::Read)) {
    localSize = lu.getLength();

     localDate = lu.getModifiedTime(time) ? time : CTime::GetCurrentTime();;

    lu.close();
    }

  return true;
  }


void SiteFile::setWebAttr() {
  siteFtp.size(remotePath(path), remoteSize); siteFtp.modDate(remotePath(path), remoteDate);
  }


void SiteFile::setLclAttr() {
String s = site.rootPath + path;
FileIO lu;
CTime  time;

  if (lu.open(s, FileIO::Read)) {
    localSize = lu.getLength();

     localDate = lu.getModifiedTime(time) ? time : CTime::GetCurrentTime();;

    lu.close();
    }
  }


bool SiteFile::getFile() {String s = site.rootPath + path;  return siteFtp.getFile(remotePath(path), s);}
bool SiteFile::putFile() {String s = site.rootPath + path;  return siteFtp.putFile(s, remotePath(path));}
bool SiteFile::delFile() {return siteFtp.delFile(remotePath(path));}



static TCchar* notExt[]   = {_T(".vpj"),
                             _T(".vpw"),
                             _T(".vpwhist"),
                             _T(".vtg")
                             };
static int     nNotExt    = noElements(notExt);


bool SiteFile::filter() {
int i;

  for (i = 0; i < nNotExt; i++) if (name.find(notExt[i]) > 0) return true;

  return false;
  }


SiteFile* SiteFiles::find(TCchar* localPath) {
FilesIter iter(*this);
SiteFile* siteFile;

  for (siteFile = iter(); siteFile; siteFile = iter++) if (siteFile->path == localPath) return siteFile;

  return 0;
  }


void SiteFiles::display() {
FilesIter iter(*this);
SiteFile* fl;

  notePad << nBeginLine << _T("Files") << nEndLine << nCrlf << nCrlf;

  for (fl = iter(); fl; fl = iter++) fl->display();
  }


void SiteFile::display() {
String d  = localDate;
String rd = remoteDate;

  notePad << name << nTab << localSize << nTab << d << nTab << path << nCrlf;
  notePad << nTab << remoteSize << nTab << rd << nCrlf;
  }


void SiteFiles::dspUpdates() {
FilesIter iter(*this);
SiteFile* fl;

  notePad << nBeginLine << _T("Update Files") << nEndLine << nCrlf << nCrlf;

  for (fl = iter(); fl; fl = iter++) fl->dspUpdate();
  }


void SiteFile::dspUpdate() {
String d  = localDate;
String rd = remoteDate;

  switch (status) {
    case PutFile: notePad << _T("Put File:");       break;
    case GetFile: notePad << _T("Get File:");       break;
    case Changed: notePad << _T("Changed File:");   break;
    case Delete : notePad << _T("Delete File:");    break;
    default     : notePad << _T("Nil Status:");     break;
    }

  if (update) notePad << _T(" -- ") << _T("Update");   notePad << nCrlf;

  notePad << name << nTab << localSize << nTab << d << nTab << path << nCrlf;
  notePad << nTab << remoteSize << nTab << rd << nCrlf;
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

      case 2  : localSize = tok.name.stoi(x); break;

      case 3  : {ToDate lcldt(tok.name);  localDate = lcldt();} break;

      case 4  : remoteSize = tok.name.stoi(x); break;

      case 5  : {ToDate rmtdt(tok.name);  remoteDate = rmtdt();} break;

//    case 6  : remotePath = tok.name; break;

      default : notePad << _T("Whoops!") << nCrlf;
      }
    lex.accept_two_tokens();
    }

  return false;
  }



void SiteFiles::save(CSVOut& csvOut) {
FilesIter iter(*this);
SiteFile* siteFile;

  for (siteFile = iter(); siteFile; siteFile = iter++) {
    siteFile->save(csvOut);
    }
  }


void SiteFile::save(CSVOut& csvOut) {

  csvOut << name       << Comma;
  csvOut << path       << Comma;
  csvOut << localSize  << Comma;

  csvOut << localDate  << Comma;
  csvOut << remoteSize << Comma;

  csvOut << remoteDate << Comma;
  csvOut << vCrlf;
  }


