// Data Storage for Ftp Operations


#pragma once
#include "ExpandableP.h"
#include "IterT.h"
#include "openssl/ssl.h"


typedef char               FtpBfr[1024];
typedef SOCKET             Socket;
typedef struct sockaddr_in SockAddrIn4;
enum                       LastOp {NilOp, ReadOp, WriteOp};


// Logging Call when #if is true (i.e. 1)
#if 0
#include "NotePad.h"
#define FtpLog(a) notePad << _T("Log: \"") << a << _T("\"") << nCrlf
#else
#define FtpLog(a)
#endif


class FtpBlk {
public:

int    curX;                              // Current Index
int    n;                                 // Number of bytes stored in buffer
FtpBfr bfr;

  FtpBlk() : n(0) { }
  FtpBlk(FtpBlk& b) {copy(b);}
 ~FtpBlk() { }

  FtpBlk& operator= (FtpBlk& b) {copy(b); return *this;}

private:

  void copy(FtpBlk& b) {memcpy(bfr, b.bfr, sizeof(FtpBfr)); n = b.n;}
  };


typedef DatumPtrT<FtpBlk> FtpBlkP;        // Usually defined just before iterator
class FtpStore;
typedef IterT<FtpStore, FtpBlk> FtpIter;  // Iterator over array of Datuss


class FtpStore {
String line;
bool   clrLn;
protected:
int                             curBlk;   // Current blocks
ExpandableP<FtpBlk, FtpBlkP, 2> data;

bool     opened;                          // connected to host when true
Socket   skt;                             // Windows Socket

// OpenSSL Data
SSL*     ssl;                             // The OpenSSL library objects needed for
SSL_CTX* ctx;                             // encrypted communication
BIO*     sbio;

// Store Data
bool     encrypt;                         // Encryption accepted
LastOp   lastOp;                          // Last Operation
String   lastResp;                        // Last Response String
Tchar    lastCh;                          // Last initial char of Response String

public:

  FtpStore() : clrLn(true), curBlk(0), opened(false), skt(0),
                                              ssl(0), ctx(0), sbio(0), encrypt(false), lastOp(NilOp) { }
 ~FtpStore() {close();  clear();}

  void clear() {curBlk = 0; data.clear();}

  bool open(TCchar* host);                // Opens Socket to host site
  bool openSSL();                         // Opens SSL connection to site

  void write();                           // Write everything in store to web host (transport function)

  void close();
  void closeSkt() {if (skt) closesocket(skt); skt = 0;}
  void closeSSL();

  int   nData()      {return data.end();} // returns number of data items in array

protected:

  bool sendCmd(TCchar* cmd, TCchar* arg = 0);
  bool readRsp(char c);

  int  write(TCchar* s);
  int  write(Cchar* buf, int noBytes);

  void read();

private:

  void clrLine()  {line.clear(); clrLn = false;}
  bool lineRdy()  {return !clrLn;}
  void saveLast() {lastResp = line;   accept();   FtpLog(lastResp);}
  void accept()   {clrLn = true;}

  bool getServerByName(Cchar* service, SockAddrIn4& sin);
  bool inetAddr(Cchar* p, u_long& addr);
  bool findHostIPaddr(Cchar* url, u_long& addr);

  bool readClr(   FtpBlk& ftpBlk);
  bool readEncrpt(FtpBlk& ftpBlk);
  bool blkd(bool blocking);
  void shutDownSkt();

  bool readBlkd(  FtpBlk& ftpBlk);
  bool readUnblkd(FtpBlk& ftpBlk);

  // returns either a pointer to data (or datum) at index i in array or zero

  FtpBlk* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  void  removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename FtpIter;
  friend class    FtpChIter;
  };


// An iterator over the "lines" in the data array.  Returns pointers to a line as String

class FtpChIter {
FtpStore& store;
int       nBlks;
FtpBlk*   blk;

public:

  FtpChIter(FtpStore& str) : store(str), blk(0) { }
 ~FtpChIter() { }

  String* operator() ();

  bool operator++ (int);
  void accept() {store.accept();}

  bool operator() (char&  ch);

private:

  bool lineRdy() {return store.lineRdy();}

  void setNBlks();
  void setCurBlk();

  FtpChIter() : store(*(FtpStore*)0) { }
  };



//  bool sendCmd( Cchar* cmd,  Cchar* arg);
//  int  write(Cchar* buf, int noBytes);
//  bool readResp(char c);
//  bool operator() (String*& s);

