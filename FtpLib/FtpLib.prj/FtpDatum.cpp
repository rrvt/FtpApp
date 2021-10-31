// FtpDatum -- Holds one 1024 data block, used


#include "stdafx.h"
#include "FtpDatum.h"


// Get next character from blk, fails when no more characters may be returned
// Removes line feed character from output

bool FtpDatum::getChar(char& ch) {

  while (i < n) {ch = blk[i++];    if (ch != '\r') return true;}

  return false;
  }

