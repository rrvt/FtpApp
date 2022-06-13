
#pragma once
#include "Date.h"
#include "FtpStore.h"    



class Ftp : public FtpStore {
public:

  Ftp();
 ~Ftp();

  bool login(TCchar* userId, TCchar* password);
  bool pwd(                  String& dir);          // Return the the current working directory
  bool dir(  TCchar* path);                         // Load directory for path into Store
  bool size( TCchar* path,   int&    size);         // File size when true
  bool date( TCchar* path,   Date&   val);          // File last modified date when true

  bool put(TCchar* webPath);                        // copy transport buffer to web host file
  bool get(TCchar* webPath);                        // copy file from web host to transport buffer
  bool del(TCchar* webPath);
  void close();

private:

  bool initPassiveMode(TCchar* cmd, TCchar* arg);
  bool isAsciiFile(TCchar* webFile);
  };


extern Ftp ftp;
