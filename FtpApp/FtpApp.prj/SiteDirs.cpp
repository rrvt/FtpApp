// Site Directory -- Note only contains the directories on the local version of the web site


#include "stdafx.h"
#include "SiteDirs.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "filename.h"
#include "FileSrch.h"
#include "NotePad.h"
#include "Site.h"
#include "SiteFiles.h"


TCchar* EndOfDirs = _T("<EndDirs>");


// Find all sub-directory names and place them in the list

void SiteDirs::findNames(TCchar* fullPath) {
String    path = fullPath;
FileSrch  srch;
String    sub;
DirIter   iter(*this);
SiteDir   siteDir;

  if (path.isEmpty()) return;

  siteDir.set(path);

  if (filter(siteDir.name)) return;

  data = siteDir;                       // Place in sorted list

  if (srch.findAllSubDirs(fullPath)) while (srch.getName(sub)) findNames(sub);
  }


static TCchar* notFiles[] = {_T("tmp"),
                             _T("_notes")
                             };
static int     nNotFiles  = noElements(notFiles);



// Some files are just not worth dealing with...

bool SiteDirs::filter(TCchar* name) {
int i;

  for (i = 0; i < nNotFiles; i++) {
    if (_tcsicmp(name, notFiles[i]) == 0) return true;
    }

  return false;
  }



void SiteDirs::display() {
DirIter  iter(*this);
SiteDir* dir;

  notePad << nBeginLine << _T("Directories") << nEndLine << nCrlf << nCrlf;

  for (dir = iter(); dir; dir = iter++) {
    notePad << dir->name << nTab << nTab << nTab << dir->path << nCrlf;
    }
  }




void SiteDir::set(TCchar* relPath) {
String pth = relPath;
int    pos = pth.length() - 1;
String p = pth[pos] == _T('\\') ? pth.substr(0, pos) : pth;

  path = getPath(p);
  name = p.substr(path.length());   path += name;
  }



bool SiteDirs::load(CSVLex& lex) {
int i;

  data.clear();

  for (i = 0; ; i++) {
    SiteDir siteDir;

    if (!siteDir.load(lex)) return i > 0;

    data = siteDir;
    }
  }



bool SiteDir::load(CSVLex& lex) {
CSVtokCode code;
CSVtokCode code1;
int        i;
bool       rslt = true;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); return rslt;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : name = tok.name;   if (name == EndOfDirs) rslt = false;
                break;

      case 1  : path = tok.name; break;

      default : notePad << _T("Whoops!") << nCrlf;
      }
    lex.accept_two_tokens();
    }

  return false;
  }


void SiteDirs::save(CSVOut& csvOut) {
DirIter  iter(*this);
SiteDir* dir;

  for (dir = iter(); dir; dir = iter++) dir->save(csvOut);

  csvOut << EndOfDirs << Comma << vCrlf;
  }


void SiteDir::save(CSVOut& csvOut) {csvOut << name << Comma << path << Comma << vCrlf;}


