// Ftp Error class

#pragma once
#include <openssl/ssl.h>


class FtpErr {

String last;

public:

  FtpErr() { }
 ~FtpErr() { }

  void    wsa(TCchar* src);
  void    skt(TCchar* src, int rslt);
  bool    ssl(SSL* ssl, int rslt);                    // Report if rslt is an ssl error


  void    put(TCchar* err) {last = err;}
  String& get()            {return last;}
  };


extern FtpErr ftpErr;
