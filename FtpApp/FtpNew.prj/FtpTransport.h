// Transport Data


#pragma once
#include "FtpStore.h"



typedef union {
struct sockaddr    sa;
struct sockaddr_in in;
} Sin;


class FtpTransport : public FtpStore {

bool   opened;                    // Connected to server

public:

  FtpTransport() : FtpStore(), opened(false) { }
 ~FtpTransport() { }

  bool open(Sin& sin);

  bool read();

  void close() {FtpStore::close();}

  bool load(TCchar* path);                 // load store buffer from local file
  bool store(TCchar* path);                // stote store buffer in a local file
  };


extern FtpTransport transport;

