// Utilities


#include "stdafx.h"
#include "Utilities.h"


String remotePath(TCchar* path) {
String s = path;
int    n = s.length();
int    i;

  for (i = 0; i < n; i++) if (s[i] == _T('\\')) s[i] = _T('/');

  return s;
  }


String localPath(TCchar* path) {
String s = path;
int    n = s.length();
int    i;

  for (i = 0; i < n; i++) if (s[i] == _T('/')) s[i] = _T('\\');

  return s;
  }





String parseName(TCchar* path) {
String s   = path;
int    pos = s.findLastOf(_T('\\'));   if (pos < 0) return s;

  return s.substr(pos + 1);
  }

