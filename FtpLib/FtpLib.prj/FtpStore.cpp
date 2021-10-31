// Ftp Store (temporary storage for data retrieved from website


#include "stdafx.h"
#include "FtpStore.h"


bool FtpStore::open(TCchar* localPath, FileIO::OpenParms mode)
                                            {String path = localPath;   return fileIO.open(path, mode);}


String* FtpStoreIter::operator() () {

  if (!initDatum(iter())) return 0;

  return getString();
  }


String* FtpStoreIter::operator++ (int) {return getString();}


String* FtpStoreIter::getString() {
char  ch;
Tchar tch;

  line.clear();

  loop {

    while (datum->getChar(ch)) {

      tch = Tchar(ch);

      if (tch == _T('\n')) return line.isEmpty() ? 0 : &line;

      line += tch;
      }

    if (!initDatum(iter++)) return line.isEmpty() ? 0 : &line;
    }
  }

