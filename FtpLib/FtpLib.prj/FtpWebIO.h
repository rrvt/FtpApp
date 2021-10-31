// Ftp Web Input/Output -- There is one of these objects for the Control and Transfer objects.
// It is customizable (more or less) to use encryption or not use encryption.


#pragma once
#include <openssl/ssl.h>

class FtpDatum;


// FtpLib Typedef Definitions

typedef SOCKET              Socket;
typedef struct sockaddr_in  SockAddrIn4;
typedef struct servent      ServerEnt;
typedef union {
struct sockaddr    sa;
struct sockaddr_in in;
} Sin;



typedef bool (*FtpCertCB)(X509* cert);          // Certificate Callback -- may not be implemented


class FtpWebIO {

enum LastOp {NilOp, ReadOp, WriteOp};

bool      encrypt;                      // Call SSL for transfers when true
Socket    skt;                          // Windows Socket
LastOp    lastOp;                       // Last operation, helps during close

SSL*      ssl;                          // The OpenSSL library objects needed for
SSL_CTX*  ctx;                          // encrypted communication
BIO*      sbio;

FtpCertCB certCB;

public:

  FtpWebIO() : encrypt(false), skt(0), lastOp(NilOp), ssl(0), ctx(0), sbio(0), certCB(0) { }
 ~FtpWebIO() { }

 // Control Initialization Functions

  void     setCertCB(FtpCertCB cbf) {certCB = cbf;}

  void     initEncryp(Socket socket);
  bool     setEncryp();

  // Data Transfer Initialization Functions

  bool     correctPasvResponse(unsigned char* v);
  bool     initDataXsfr();
  bool     connect(Sin& sin);
  SSL_CTX* context() {return ctx;}

  // Control & Data transfer Functions

  int      read(FtpDatum& datum);

  int      write(Cchar* buf, int noBytes);

  bool     certCallback();

  bool     close();
  void     closeSocket() {if (skt) {closesocket(skt); skt = 0;}}

  bool     isSSLerror(int rslt);

private:

  void     closeSSL();
  };




