// Data Storage for Ftp Operations


#include "stdafx.h"
#include "FtpStore.h"
#include "FtpErr.h"
#include <ws2tcpip.h>


typedef struct servent      ServerEnt;
typedef struct in_addr      InAddr;


// Opens Socket to site

bool FtpStore::open(TCchar* host) {
String      web;
SockAddrIn4 sin;
int         on = 1;
char        lhost[1024];
char*       pos;
int         rslt;

  memset(&sin, 0, sizeof(sin));   sin.sin_family = AF_INET;

  web = _T("ftp."); web += host;   ToAnsi h(web);

  strcpy_s(lhost, sizeof(lhost), h());    pos = strchr(lhost, ':');

  if (!getServerByName("ftp", sin)) return false;

  if (!inetAddr(lhost, sin.sin_addr.s_addr) && !findHostIPaddr(lhost, sin.sin_addr.s_addr)) return false;

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (skt == INVALID_SOCKET) {ftpErr.wsa(_T("::socket")); skt = 0; return false;}

  rslt = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on));
  if (rslt ==  SOCKET_ERROR ) {ftpErr.wsa(_T("setsockopt")); closeSkt(); return false;}

  if (::connect(skt, (SockAddr*) &sin, sizeof(sin)))
                                      {ftpErr.wsa(_T("connect")); closeSkt(); return false;}
  opened = true;

  return openSSL();
  }




bool FtpStore::getServerByName(Cchar* service, SockAddrIn4& sin) {
ServerEnt* pse = getservbyname(service, "tcp");

  if (!pse) {ftpErr.wsa(_T("getservbyname"));  return false;}

  sin.sin_port = pse->s_port;    return true;
  }


bool FtpStore::inetAddr(Cchar* p, u_long& addr) {
InAddr inAddr;

  if (inet_pton(AF_INET, p, &inAddr) <= 0)  return false;

  addr = inAddr.S_un.S_addr;    return true;
  }


typedef struct addrinfo     AddrInfo;
typedef short               AddrFamily;
typedef struct sockaddr_in6 SockAddrIn6;


bool FtpStore::findHostIPaddr(Cchar* url, u_long& addr) {
AddrInfo     hints;
AddrInfo*    info;
char         stringBuf[46];
AddrFamily   family;
SockAddrIn4* sockaddr_ipv4;
SockAddrIn6* sockaddr_ipv6;
int          rslt;

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  rslt = GetAddrInfoA(url, "http", &hints, &info);

  if (rslt) {ftpErr.skt(_T("findHostIPaddr"), rslt);  return false;}

  stringBuf[0] = 0;  family = info->ai_family;

  switch (family) {
    case AF_UNSPEC: ftpErr.put(_T("Family Unspecified"));   return false;

    case AF_INET  : sockaddr_ipv4 = (SockAddrIn4*) info->ai_addr;

                    addr = sockaddr_ipv4->sin_addr.s_addr; break;

    case AF_INET6 : sockaddr_ipv6 = (SockAddrIn6*) info->ai_addr;

                    sockaddr_ipv6->sin6_addr;

                    ftpErr.put(_T("Family: IP6 address"));  return false;

                    break;

    default       : ftpErr.put(_T("Gsmily: Other format")); return false;
    }

  return true;
  }


bool FtpStore::openSSL() {

  if (!skt) return false;
  if (ssl)  return true;

  SSL_library_init();

  ctx = SSL_CTX_new(TLS_client_method());   SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

  ssl = SSL_new(ctx);

  if (!readRsp('2')) {close(); return false;}

  if (!sendCmd(_T("AUTH TLS")) || !readRsp('2')) return false;

  sbio = BIO_new_socket(skt, BIO_NOCLOSE);

  SSL_set_bio(ssl, sbio, sbio);

  if (ftpErr.ssl(ssl, SSL_connect(ssl))) return false;

  return encrypt = true;
  }


void FtpStore::close() {closeSSL();   shutDownSkt();   closeSkt();  clear();  lastOp = NilOp;}


void FtpStore::closeSSL() {

  if (!ssl) return;

  SSL_shutdown(ssl);   SSL_free(ssl);   SSL_CTX_free(ctx);

  encrypt = false;   ssl = 0;   ctx = 0;   sbio = 0;
  }


void FtpStore::shutDownSkt() {

  if (lastOp == WriteOp && !encrypt) {

    if (skt && shutdown(skt, SD_SEND)) ftpErr.wsa(_T("Shutdown"));

    read();
    }
  }


// The command and arguments are combined (provided the arg pointer is not zero) and the command is sent
// Clears the response buffer and returns result of send the command

bool FtpStore::sendCmd(TCchar* cmd, TCchar* arg) {
String s = cmd;

  if (arg) {s += _T(' ');   s += arg;}

  FtpLog(s);   s += _T("\r\n");

  return write(s) == s.length();
  }


void FtpStore::write() {
FtpIter iter(*this);
FtpBlk* blk;

  for (blk = iter(); blk; blk = iter++) write(blk->bfr, blk->n);
  }


// Write a line to the web host

int FtpStore::write(TCchar* s) {ToAnsi buf(s);   return write(buf(), buf.length());}


// Write the data in the buffer to the web host using the correct method

int FtpStore::write(Cchar* buf, int noBytes) {
int n;

  if (encrypt) {n = SSL_write(ssl, buf, noBytes);  if (ftpErr.ssl(ssl, n)) return n;}
  else         {n = send(skt, buf, noBytes, 0);    if (n < 0) {ftpErr.wsa(_T("send")); return n;}}

  lastOp = WriteOp;  return n;
  }


// read a response from the server, logs it and checks the first character against the character c
// Succeeds when first character is equal to c

bool FtpStore::readRsp(char c) {
FtpChIter iter(*this);
String    match;

  lastResp.clear();   lastCh = 0;

  iter();   if (!iter++) return false;

  saveLast();

  if (line.length() > 3 && line[3] == _T('-'))
                       for (match = line.substr(0, 3); iter++ && line.substr(0,3) == match; ) saveLast();

  if (lastResp.length() > 0) lastCh = lastResp[0];

  return lastCh == Tchar(c);
  }



// Logic for reading the buffers

String* FtpChIter::operator() () {
  setNBlks();   setCurBlk();

  if (!blk || blk->curX >= blk->n) {store.read();   setNBlks();   setCurBlk();}

  return &store.line;
  }


#if 0
bool FtpChIter::operator() (String*& s) {
char ch;

  s = &store.line;

  if (lineRdy()) return true;

  for (store.clrLine(); (*this)(ch); ) {
    if (ch == '\n') return true;

    store.line += Tchar(ch);
    }

  if (store.line.isEmpty()) {accept(); return false;}

  return true;
  }
#endif

bool FtpChIter::operator++ (int) {
char ch;

  if (lineRdy()) return true;

  for (store.clrLine(); (*this)(ch); ) {
    if (ch == '\n') return true;

    store.line += Tchar(ch);
    }

  if (store.line.isEmpty()) {accept(); return false;}

  return true;
  }



bool FtpChIter::operator() (char& ch) {
  if (!blk) return false;
  if (blk->curX >= blk->n) {
    if (++store.curBlk >= nBlks) return false;

    setCurBlk();   return (*this)(ch);
    }

  ch = blk->bfr[blk->curX++];   if (ch == '\r') return (*this)(ch);

  return true;
  }


void FtpChIter::setCurBlk() {blk = store.curBlk < nBlks ? store.data[store.curBlk].p : 0;}


void FtpChIter::setNBlks() {
  nBlks = store.nData();

  if (store.curBlk >= nBlks) {store.read(); nBlks = store.nData();}
  }


void FtpStore::read() {
int i;

  data.clear();   blkd(true);   curBlk = 0;

  for (i = 0; ; i++) {
    data[i].p = data.allocate();
    FtpBlk& blk = *data[i].p;

    if (encrypt) {if (!readEncrpt(blk)) break;}
    else         {if (!readClr(   blk)) break;}
    }

  lastOp = ReadOp;  blkd(true);
  }


bool FtpStore::readClr(FtpBlk& ftpBlk) {
char* bfr  = ftpBlk.bfr;
int   size = sizeof(FtpBfr);
int   n;
int   i;

  ftpBlk.n = ftpBlk.curX = 0;

  for (i = 0, n = recv(skt, bfr, size, 0); n > 0 && i < 10;
                                                i++, n = recv(skt, bfr + ftpBlk.n, size - ftpBlk.n, 0)) {
    ftpBlk.n += n;   blkd(false);   if (n >= size) break;   Sleep(100);
    }

  return ftpBlk.n == size;
  }


bool FtpStore::readEncrpt(FtpBlk& ftpBlk) {
char*  bfr  = ftpBlk.bfr;
size_t size = sizeof(FtpBfr);
size_t n;

  ftpBlk.n = ftpBlk.curX = 0;

  for (n = SSL_read(ssl, bfr, size); (int) n > 0; n = SSL_read(ssl, bfr + ftpBlk.n, size - ftpBlk.n)) {
    ftpBlk.n += n;   blkd(false);   if (n >= size) break;   Sleep(100);
    }

  return ftpBlk.n == size;
  }


// enables or disables the blocking mode for the
// socket based on the numerical value of iMode.
// If iMode = 0, blocking is enabled;
// If iMode != 0, non-blocking mode is enabled.
/*
  iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
  if (iResult != NO_ERROR) printf("ioctlsocket failed with error: %ld\n", iResult);
*/

bool FtpStore::blkd(bool blocking) {
ulong arg = blocking ? 0 : 1;

  if (!skt) return false;

  if (!ioctlsocket(skt, FIONBIO, &arg)) return true;

  ftpErr.wsa(_T("ioctlsocket failed with error")); return false;
  }





#if 0
// Blocking Read == blocks until read is ready
// Set the socket I/O mode: In this case FIONBIO

bool FtpStore::readBlkd(FtpBlk& ftpBlk) {
char*  bfr  = ftpBlk.bfr;
int    size = sizeof(FtpBfr);
size_t n;

  blkd(true);   ftpBlk.n = 0;

  n = recv(skt, bfr, size, 0);    if (n < 0) {ftpErr.wsa(_T("recv")); return false;}}

  ftpBlk.n = n;   return n == size;
  }


bool FtpStore::readUnblkd(FtpBlk& ftpBlk) {
char*  bfr  = ftpBlk.bfr;
int    size = sizeof(FtpBfr);
size_t n;
int    i;

  blkd(false);   ftpBlk.n = 0;

  for (i = 0; i < 10; i++) {

    if (encrypt) {if (!SSL_read_ex(ssl, bfr + ftpBlk.n, size - ftpBlk.n, &n)) break;}

    else {n = recv(skt, bfr + ftpBlk.n, size - ftpBlk.n, 0);   if (n <= 0) break;}

    ftpBlk.n += n;   Sleep(100);
    }

  return ftpBlk.n == size;
  }
//    else if (!n) {if (!readBlkd(  blk)) {n++; break;}}
//    else    {     if (!readUnblkd(blk)) {n++; break;}}
#endif
#if 0
bool FtpStore::sendCmd(Cchar* cmd, Cchar* arg) {
char cmdBuf[256];
int  n;

  strcpy_s(cmdBuf, sizeof(cmdBuf), cmd);

  if (arg) {strcat_s(cmdBuf, sizeof(cmdBuf), " ");   strcat_s(cmdBuf, sizeof(cmdBuf), arg);}

  strcat_s(cmdBuf, sizeof(cmdBuf), "\r\n");

  n = strlen(cmdBuf);   return write(cmdBuf, n) == n;
  }


// Write the data in the buffer to the web host using the correct method

int FtpStore::write(Cchar* buf, int noBytes) {
int n;

  if (encrypt) {n = SSL_write(ssl, buf, noBytes);  if (ftpErr.ssl(ssl, n)) return n;}
  else         {n = send(skt, buf, noBytes, 0);    if (n < 0) {ftpErr.wsa(_T("send")); return n;}}

  lastOp = WriteOp;  return n;
  }
#endif





#if 0
// read a response from the server, logs it and checks the first character against the character c
// Succeeds when first character is equal to c

bool FtpStore::readResp(char c) {
FtpChIter iter(*this);
String    line;
Tchar     firstCh;
Tchar     tch;
String    match;

  iter();   lastResp.clear();   lastCh = 0;   if (!iter(line)) return false;

  lastResp = line;   FtpLog(lastResp);

  if (line[3] == '-') {

    match = line.substr(0, 3);   firstCh = line[0];

    while (iter.peek(tch) && tch == firstCh && iter(line))
      {lastResp = line;   FtpLog(lastResp);   if (match != line.substr(0,3)) break;}
    }

  if (lastResp.length() > 0) lastCh = lastResp[0];

  return lastCh == Tchar(c);
  }
#endif


#if 0
bool FtpChIter::operator() (String& s) {
char ch;

  for (s.clear(); (*this)(ch); ) {
    if (ch == '\n') return true;

    s += Tchar(ch);
    }

  return !s.isEmpty();
  }
#endif


#if 0
bool FtpChIter::peek(Tchar& tch) {
  if (!blk) return false;
  if (blk->curX >= blk->n) {
    if (++store.curBlk >= nBlks) return false;

    setCurBlk();   return peek(tch);
    }

  tch = Tchar(blk->bfr[blk->curX]);   if (tch == _T('\r')) {blk->curX++; return peek(tch);}

  return true;
  }
#endif

