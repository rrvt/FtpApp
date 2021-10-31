// Ftp Transfer Class -- does all the work of transferring data
// This is the other class for interfacing with the web host.  This one moves data.


#include "stdafx.h"
#include "FtpTransfer.h"


// The web host sends a detailed response and it is stored in a data structure in ftpStore
// When the operation completes the store determines if there was a successful transfer

bool FtpTransfer::webDataToStore(FtpStore& ftpStore) {

  while (webPort.webDataToStore(ftpStore)) continue;

  return !ftpStore.isEmpty();
  }


// Store web data in file
// The theory of success is that at least one block will be transferred without failure so the counter
// will be at least one

bool FtpTransfer::webDataToFile(FtpStore& ftpStore) {
int noBlks;

  for (noBlks = 0; webPort.webDataToFile(ftpStore); noBlks++) continue;

  return noBlks;
  }


// Store local file data in Web file
// See above for success theory...

bool FtpTransfer::fileDataToWeb(FtpStore& ftpStore) {
int noBlks;

  for (noBlks = 0; webPort.fileDataToWeb(ftpStore); noBlks++) continue;

  return noBlks;
  }

