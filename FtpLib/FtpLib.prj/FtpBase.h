// Ftp Base Class for Control and Transfer


#pragma once
#include "FtpWebPort.h"


class FtpBase {
public:

FtpWebPort webPort;                             // implements a buffer and read/write operatons on it

long       offset;
bool       correctPasv;

  FtpBase() : offset(0), correctPasv(false) { }
 ~FtpBase() { }

  virtual bool close() = 0;
  };

