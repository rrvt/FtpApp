// Utilities --


#pragma once
#include "IniFile.h"



String remotePath(TCchar* path);      // Replace '\' with '/'
String localPath(TCchar* path);       // Replace '/' with '\'
String parseName(TCchar* path);       // Finds the name after the last '\'
void   fixRootPath(String& root);

class Baffle : protected IniFile {
String es;
public:

  Baffle() { }
 ~Baffle() { }

  void clear() {es.clear();}

  String& operator<< (String& s)   {return es = encodePassword(s);}
  String& operator<< (CString& cs) {String s = cs; return *this << s;}
  String  operator>> (String& s)   {return s  = decodePassword(es);}
  String  operator>> (CString& cs) {String s; *this >> s; cs = s; return s;}
  String  operator() ()            {return es;}
  String& operator=  (String& s)   {return es = s;}
  };



