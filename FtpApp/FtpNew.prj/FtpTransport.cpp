// Transport Data


#include "stdafx.h"
#include "FtpTransport.h"
#include "FileIO.h"
#include "FtpErr.h"
//#include <ws2tcpip.h>


FtpTransport transport;


typedef struct linger  Linger;


bool FtpTransport::open(Sin& sin) {
int    on  = 1;
Linger lng = {0, 1000};

  if (skt) closeSkt();

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (skt == INVALID_SOCKET) {ftpErr.wsa(_T("Transport Socket"));   skt = 0;   return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on)) == -1)
                                  {ftpErr.wsa(_T("Transport Set Sockopt"));  closeSkt(); return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_LINGER, (Cchar*) &lng,sizeof(lng)) == -1)
                                 {ftpErr.wsa(_T("Transport Set Linger"));   closeSkt();  return false;}

  opened = !::connect(skt, &sin.sa, sizeof(sin.sa));

  if (!opened) {ftpErr.wsa(_T("Transport Connect Failed"));   closeSkt();}

  return opened;
  }


bool FtpTransport::read() {

  clear();

  if (!opened) return false;

  FtpStore::read(); return true;
  }


// load store buffer from local file

bool FtpTransport::load(TCchar* path) {
FileIO  fio;
FtpBlk* blk;

  clear();

  if (!fio.open(path, FileIO::Read)) return false;

  for (blk = data.allocate(); blk; blk = data.allocate()) {
    blk->n = fio.read(blk->bfr, sizeof(FtpBfr));

    if (blk->n == 0) {data.deallocate(blk); break;}

    data += blk;
    }

  fio.close();   return data.end() > 0;
  }


// stote store buffer in a local file

bool FtpTransport::store(TCchar* path) {
FileIO  fio;
FtpIter iter(*this);
FtpBlk* blk;
int     mode = FileIO::Write | FileIO::Create;
bool    rslt = true;

  if (!fio.open(path, (FileIO::OpenParms) mode)) return false;

  for (blk = iter(); blk; blk = iter++) {
    if (!fio.write(blk->bfr, blk->n)) {rslt = false; break;}
    }

  fio.close();   return rslt;
  }

