// FtpDatum -- Holds one 1024 data block, used almost everywhere to hold data moving between th local
// and remote sites.


#pragma once


typedef char FtpBuffer[1024];


class FtpDatum {
int       i;
int       n;
FtpBuffer blk;

public:

  FtpDatum() : i(0), n(0) { }
 ~FtpDatum() { }

  void  clear() {n = i = 0;}

  char* buf()            {return blk;}
  int   size()           {return sizeof(blk);}
  bool  set(int noBytes) {i = 0; n = noBytes; return noBytes == sizeof(blk);}   // Return true when full

  bool  isEmpty()        {return n == 0 || i >= n;}

  bool  getChar(char& ch);

private:

  void  initRead() {i = 0;}

  friend class FtpStoreIter;
  };

