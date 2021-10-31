// Ftp Library -- Main Interface
// The Application should include this where it needs access to the web host.
// Two other may need to be included in an application:
//   * FtpStore -- Holds the results of the dir and nameList commands
//   * FtpCmdLog -- Needed if the application needs to deal with the detail log of operations


#pragma once
#include "Date.h"
#include "FtpControl.h"


class FtpLib {

FtpControl control;

public:

  FtpLib();
 ~FtpLib() {WSACleanup();}

  bool   connect(Cchar* host) {return control.connect(host);}

  bool   login(Cchar* user, Cchar* pass);               // {return control.login(user, pass);}

  bool   sysType(String& systemType);
  bool   sysType( char* buf, int lng);

  bool   nameList(Cchar* webPath, FtpStore& ftpStore);  // Data is placed in the FtpStore3
  bool   dir(     Cchar* webPath, FtpStore& ftpStore);  // Data is placed in the FtpStore3

  // directory Manipulation

  bool   pwd(String& dir);                            // return current remote directory
  bool   pwd(char* buf, int noBytes);
  bool   chDir(Cchar* webPath);                       // Change Directory, either full path or new subdir
  bool   cdUp();                                      // Change Directory to parent of current directory
  bool   mkDir(Cchar* webPath);                       // Make a new Remote Directory
  bool   rmDir(Cchar* webPath);                       // Remove a Remote Directory

  // File Attributes & Operations

  bool   size(   Cchar* webPath, int& size);          // size becomes the file size when returns true
  bool   modDate(Cchar* webPath, Date& date);
  bool   ren(    Cchar* webSrc,  Cchar* webDst);      // rename file from src to dst in current directory
  bool   del(    Cchar* webPath);                     // Delete file on remote site
  bool   get(    Cchar* webPath,  TCchar* localPath, FtpStore& ftpStore);
  bool   put(   TCchar* localPath, Cchar* webPath,   FtpStore& ftpStore);

  bool   isAsciiFile(Cchar* webFile);

  String lastResponse() {return control.partialResp(0);}

  bool   close() {return control.close();}

  void   setCertCB(FtpCertCB cbf) {control.setCertCB(cbf);}

private:

  bool   sendCmd(Cchar* cmd,             char resp) {return control.sendCmd(cmd,      resp);}
  bool   sendCmd(Cchar* cmd, Cchar* arg, char resp) {return control.sendCmd(cmd, arg, resp);}
  };

