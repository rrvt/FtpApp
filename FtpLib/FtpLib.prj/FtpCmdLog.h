// Ftp Command/Response Log
//
// When the application wishes to view the internal working of FtpLib the application needs to call
// SetCB:
//
//  ftpCmdLog.setCB(callBackFunction);
//
// The callBackFunction needs to be declared as described by the typedef below.


#pragma once


typedef void (*FtpCmdLogCB)(Cchar* str, bool isCmd);     // Call Back for displaying commands and their
                                                         // respones

class FtpCmdLog {

FtpCmdLogCB callBack;

public:

  FtpCmdLog() : callBack(0) { }
 ~FtpCmdLog() { }

  void setCB(FtpCmdLogCB callBack) {this->callBack = callBack;}

  void stopLogging() {callBack = 0;}

  void operator() (Cchar* stg, bool isCmd) {if (callBack) callBack(stg, isCmd);}
  };


extern FtpCmdLog ftpCmdLog;         // The log object.
