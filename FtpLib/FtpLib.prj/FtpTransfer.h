// Ftp Transfer Class -- does all the work of transferring data
// This is the other class for interfacing with the web host.  This one moves data.


#pragma once
#include "FtpBase.h"


class FtpTransfer : public FtpBase {
public:

  FtpTransfer() { }
 ~FtpTransfer() {close();}

  // Web data stored in data array

  bool webDataToStore(FtpStore& ftpStore);            // Store web data in a data structure, used for
                                                      // commands that need response of data
  // File Operations

  bool webDataToFile(FtpStore& ftpStore);             // These are used to move files from one site to
  bool fileDataToWeb(FtpStore& ftpStore);             // the other site.

  bool close() {return webPort.close();}
  };

