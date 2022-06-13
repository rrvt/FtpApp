// Ftp Error class


#include "stdafx.h"
#include "FtpErr.h"
#include "MessageBox.h"


FtpErr ftpErr;


void FtpErr::wsa(TCchar* src) {
int    err = WSAGetLastError();
String s;

  getError(err, s);   last = src;   last += _T(" -- ") + s;   messageBox(last);
  }


void FtpErr::skt(TCchar* src, int rslt) {
String s;

  getError(rslt, s);  last = src;  last += _T(" -- ") + s;   messageBox(last);
  }


// Note comments below this function for more information, the comment having been copied from the
// OpenSSL source code

bool FtpErr::ssl(SSL* ssl, int rslt) {
int    err = SSL_get_error(ssl, rslt);
String s = _T("SSL error:  ");

  switch (err) {
    case SSL_ERROR_NONE                 : return false;
    case SSL_ERROR_ZERO_RETURN          : s += _T("SSL_ERROR_ZERO_RETURN");          break;
    case SSL_ERROR_WANT_READ            : s += _T("SSL_ERROR_WANT_READ");            break;
    case SSL_ERROR_WANT_WRITE           : s += _T("SSL_ERROR_WANT_WRITE");           break;
    case SSL_ERROR_WANT_CONNECT         : s += _T("SSL_ERROR_WANT_CONNECT");         break;
    case SSL_ERROR_WANT_ACCEPT          : s += _T("SSL_ERROR_WANT_ACCEPT");          break;
    case SSL_ERROR_WANT_X509_LOOKUP     : s += _T("SSL_ERROR_WANT_X509_LOOKUP");     break;
    case SSL_ERROR_WANT_ASYNC           : s += _T("SSL_ERROR_WANT_ASYNC");           break;
    case SSL_ERROR_WANT_ASYNC_JOB       : s += _T("SSL_ERROR_WANT_ASYNC_JOB");       break;
    case SSL_ERROR_WANT_CLIENT_HELLO_CB : s += _T("SSL_ERROR_WANT_CLIENT_HELLO_CB"); break;
    case SSL_ERROR_SYSCALL              : s += _T("SSL_ERROR_SYSCALL");              break;
    case SSL_ERROR_SSL                  : s += _T("SSL_ERROR_SSL");                  break;
    default                             : s += _T("Unknown SSL Error");              break;
    }

  last = s; messageBox(last);   return true;
  }

