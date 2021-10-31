// FtpWebPort -- Interface to the low level webIO module.  Manages buffered reads and writes


#pragma once
#include "FtpWebIO.h"
#include "FtpStore.h"


class FtpWebPort {

FtpWebIO  webIO;                  // Low level IO to web host
char      lastCh;                 // Last character read
FtpDatum  datum;

public:

  FtpWebPort() { }   // : rdp(buf), wtp(buf), endP(buf + sizeof(buf))
 ~FtpWebPort() { }

 // Control Initialization Functions

  void     initEncryp(Socket socket) {webIO.initEncryp(socket);}
  bool     setEncryp() {       return webIO.setEncryp();}

  void     initResponse() {datum.clear();}        // rdp = wtp = buf;

  // Data Transfer Initialization Functions

  bool     correctPasvResponse(unsigned char* v) {return webIO.correctPasvResponse(v);}
  bool     initDataXsfr() {                       return webIO.initDataXsfr();}
  bool     connect(Sin& sin) {                    return webIO.connect(sin);}
  SSL_CTX* context() {                            return webIO.context();}

  // Control transfer Functions

  bool     write(Cchar* blk, int noBytes) {return webIO.write(blk, noBytes);}   // Send Command
  bool     readLine(char* blk, int noBytes);                                    // Read Command Response
  bool     webDataToStore(FtpStore& ftpStore);    // Read dir and nameList response, requires extensable
                                                  // storage so uses the transfer object
  // Web/File operations -- useful for get and put

  bool     webDataToFile(FtpStore& ftpStore);     // Put file to a file on host
  bool     fileDataToWeb(FtpStore& ftpStore);     // Get file from host and store in local file

  bool     close() {return webIO.close();}

  void     setCertCB(FtpCertCB cbf) {webIO.setCertCB(cbf);}

private:

  char     get();
  bool     fillBuf();                             // Response may take more than one buffer...
  };

