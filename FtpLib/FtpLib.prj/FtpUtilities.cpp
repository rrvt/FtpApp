// FtpLib Utilities


#include "stdafx.h"
#include "FtpUtilities.h"
#include "MessageBox.h"


static String lastError;


// Some calls to Windows have specific error protocols, each function translates the error
// into something meaningful to the user and displays it.  These are usually fatal errors allthough
// not much is done to deal with fatal errors...

void ftpWSAErr(TCchar* src) {
int    err = WSAGetLastError();
String s;

  getError(err, s);   lastError = src;   lastError += _T(" -- ") + s;   messageBox(lastError);
  }


void ftpSktErr(TCchar* src, int rslt) {
String s;

  getError(rslt, s);  lastError = src;  lastError += _T(" -- ") + s;   messageBox(lastError);
  }


void    ftpPutError(Cchar* err) {lastError = err;}


String& ftpGetError() {return lastError;}

