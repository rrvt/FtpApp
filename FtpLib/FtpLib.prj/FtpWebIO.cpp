// Ftp Web Input/Output -- There is one of these objects for each of the Control and Transfer objects.
// It is customizable (more or less) to use encryption or not use encryption.


#include "stdafx.h"
#include "FtpWebIO.h"
#include "FtpDatum.h"
#include "FtpUtilities.h"
#include "notePad.h"


static const TCchar* Prefix = _T("<WebIO Error>");


typedef struct linger  Linger;
typedef struct timeval TimeVal;


// Initialize encryption, create an instance of the ssl library

void FtpWebIO::initEncryp(Socket socket) {

  skt = socket;

  ctx = SSL_CTX_new(TLS_client_method());   SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

  ssl = SSL_new(ctx);
  }


// Sets up a new transfer socket for the SSL Library

bool FtpWebIO::setEncryp() {

  sbio = BIO_new_socket(skt, BIO_NOCLOSE);

  SSL_set_bio(ssl, sbio, sbio);

  if (isSSLerror(SSL_connect(ssl))) return false;

  encrypt = true;

  if (!certCallback()) return false;          // This is not emplemented in any meaningful way...

  return encrypt;
  }


// Not tested an unneeded with my web site...

bool FtpWebIO::correctPasvResponse(unsigned char* v) {
SockAddr sockAdr;
int      lng = sizeof(sockAdr);

  if (getpeername(skt, &sockAdr, &lng) == -1) {ftpWSAErr(_T("getpeername"));   return false;}

  for (int i = 2; i < 6; i++) v[i] = sockAdr.sa_data[i];

  return true;
  }


// Initialize socket for transfer, note the WSA error calls due to windows protocols

bool FtpWebIO::initDataXsfr() {
int    on  = 1;
Linger lng = {0, 1000};

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  if (skt == -1)
                                          {ftpWSAErr(_T("socket"));  return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on)) == -1)
                                          {ftpWSAErr(_T("setsockopt"));  closeSocket(); return false;}

  if (setsockopt(skt, SOL_SOCKET, SO_LINGER, (Cchar*) &lng,sizeof(lng)) == -1)
                                          {ftpWSAErr(_T("setsockopt"));   closeSocket();  return false;}
  return true;
  }


// Connect for this socket

bool FtpWebIO::connect(Sin& sin) {

  if (!::connect(skt, &sin.sa, sizeof(sin.sa))) return true;

  ftpWSAErr(_T("::connect"));   return false;
  }


// Blocking Read == blocks until read is ready

int FtpWebIO::read(FtpDatum& datum) {
char* buf  = datum.buf();
int   size = datum.size();
int   n;

  if (encrypt) {n = SSL_read(ssl, buf, size);   if (isSSLerror(n)) return n;}
  else         {n = recv(skt, buf, size, 0);    if (n < 0) {ftpWSAErr(_T("recv")); return n;}}

  lastOp = ReadOp;   return n;
  }


// Write the data in the buffer to the web host using the correct method

int FtpWebIO::write(Cchar* buf, int noBytes) {
int n;

  if (encrypt) {n = SSL_write(ssl, buf, noBytes);  if (isSSLerror(n)) return n;}
  else         {n = send(skt, buf, noBytes, 0);    if (n < 0) {ftpWSAErr(_T("send")); return n;}}

  lastOp = WriteOp;  return n;
  }


// Retrieve the certificate and send it to the call back -- Untested

bool FtpWebIO::certCallback() {
X509* cert;

  if (certCB == NULL) return true;

  cert = SSL_get_peer_certificate(ssl);

  if (!certCB(cert)) return false;

  X509_free(cert); return true;
  }


// Close out the transfer, clearing the channel if necessary

bool FtpWebIO::close() {

  if (ssl) SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);

  if (lastOp == WriteOp && !encrypt) {
    int n;
    char buf[256];

    if (skt && shutdown(skt, SD_SEND)) ftpWSAErr(_T("Shutdown"));

    for (n = recv(skt, buf, sizeof(buf), 0); n; n = recv(skt, buf, sizeof(buf), 0)) continue;
    }

  closesocket(skt); skt = 0;    closeSSL();  lastOp = NilOp;

  return true;
  }


// Close the ssl library, free things, etc...

void FtpWebIO::closeSSL()
              {encrypt = false;   SSL_free(ssl);   SSL_CTX_free(ctx);   ssl = 0;   ctx = 0;   sbio = 0;}


// Note comments below this function for more information, the comment having been copied from the
// OpenSSL source code

bool FtpWebIO::isSSLerror(int rslt) {
int err = SSL_get_error(ssl, rslt);

  switch (err) {
    case SSL_ERROR_NONE                 : return false;
    case SSL_ERROR_ZERO_RETURN          : notePad << _T("SSL_ERROR_ZERO_RETURN") << nCrlf; return true;
    case SSL_ERROR_WANT_READ            : notePad << _T("SSL_ERROR_WANT_READ") << nCrlf; return true;
    case SSL_ERROR_WANT_WRITE           : notePad << _T("SSL_ERROR_WANT_WRITE") << nCrlf; return true;
    case SSL_ERROR_WANT_CONNECT         : notePad << _T("SSL_ERROR_WANT_CONNECT") << nCrlf; return true;
    case SSL_ERROR_WANT_ACCEPT          : notePad << _T("SSL_ERROR_WANT_ACCEPT") << nCrlf; return true;
    case SSL_ERROR_WANT_X509_LOOKUP     : notePad << _T("SSL_ERROR_WANT_X509_LOOKUP") << nCrlf;
                                          return true;
    case SSL_ERROR_WANT_ASYNC           : notePad << _T("SSL_ERROR_WANT_ASYNC") << nCrlf; return true;
    case SSL_ERROR_WANT_ASYNC_JOB       : notePad << _T("SSL_ERROR_WANT_ASYNC_JOB") << nCrlf; return true;
    case SSL_ERROR_WANT_CLIENT_HELLO_CB : notePad << _T("SSL_ERROR_WANT_CLIENT_HELLO_CB") << nCrlf;
                                          return true;
    case SSL_ERROR_SYSCALL              : notePad << _T("SSL_ERROR_SYSCALL") << nCrlf; return true;
    case SSL_ERROR_SSL                  : notePad << _T("SSL_ERROR_SSL") << nCrlf; return true;
    default                             : notePad << _T("Unknown SSL Error") << nCrlf; return true;
    }

  return false;
  }


/*

SSL_ERROR_NONE

    The TLS/SSL I/O operation completed. This result code is returned if and only if ret > 0.

SSL_ERROR_ZERO_RETURN

    The TLS/SSL peer has closed the connection for writing by sending the close_notify alert. No more
    data can be read. Note that SSL_ERROR_ZERO_RETURN does not necessarily indicate that the underlying
    transport has been closed.

    This error can also appear when the option SSL_OP_IGNORE_UNEXPECTED_EOF is set. See
    SSL_CTX_set_options(3) for more details.

SSL_ERROR_WANT_READ, SSL_ERROR_WANT_WRITE

    The operation did not complete and can be retried later.

    SSL_ERROR_WANT_READ is returned when the last operation was a read operation from a nonblocking BIO.
    It means that not enough data was available at this time to complete the operation. If at a later
    time the underlying BIO has data available for reading the same function can be called again.

    SSL_read() and SSL_read_ex() can also set SSL_ERROR_WANT_READ when there is still unprocessed data
    available at either the SSL or the BIO layer, even for a blocking BIO. See SSL_read(3) for more
    information.

    SSL_ERROR_WANT_WRITE is returned when the last operation was a write to a nonblocking BIO and it was
    unable to sent all data to the BIO. When the BIO is writable again, the same function can be called
    again.

    Note that the retry may again lead to an SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE condition. There
    is no fixed upper limit for the number of iterations that may be necessary until progress becomes
    visible at application protocol level.

    It is safe to call SSL_read() or SSL_read_ex() when more data is available even when the call that
    set this error was an SSL_write() or SSL_write_ex(). However, if the call was an SSL_write() or
    SSL_write_ex(), it should be called again to continue sending the application data. If you get
    SSL_ERROR_WANT_WRITE from SSL_write() or SSL_write_ex() then you should not do any other operation
    that could trigger IO other than to repeat the previous SSL_write() call.

    For socket BIOs (e.g. when SSL_set_fd() was used), select() or poll() on the underlying socket can be
    used to find out when the TLS/SSL I/O function should be retried.

    Caveat: Any TLS/SSL I/O function can lead to either of SSL_ERROR_WANT_READ and SSL_ERROR_WANT_WRITE.
    In particular, SSL_read_ex(), SSL_read(), SSL_peek_ex(), or SSL_peek() may want to write data and
    SSL_write() or SSL_write_ex() may want to read data. This is mainly because TLS/SSL handshakes may
    occur at any time during the protocol (initiated by either the client or the server); SSL_read_ex(),
    SSL_read(), SSL_peek_ex(), SSL_peek(), SSL_write_ex(), and SSL_write() will handle any pending
    handshakes.

SSL_ERROR_WANT_CONNECT, SSL_ERROR_WANT_ACCEPT

    The operation did not complete; the same TLS/SSL I/O function should be called again later. The
    underlying BIO was not connected yet to the peer and the call would block in connect()/accept(). The
    SSL function should be called again when the connection is established. These messages can only
    appear with a BIO_s_connect() or BIO_s_accept() BIO, respectively. In order to find out, when the
    connection has been successfully established, on many platforms select() or poll() for writing on the
    socket file descriptor can be used.

SSL_ERROR_WANT_X509_LOOKUP

    The operation did not complete because an application callback set by SSL_CTX_set_client_cert_cb()
    has asked to be called again. The TLS/SSL I/O function should be called again later. Details depend
    on the application.

SSL_ERROR_WANT_ASYNC

    The operation did not complete because an asynchronous engine is still processing data. This will
    only occur if the mode has been set to SSL_MODE_ASYNC using SSL_CTX_set_mode(3) or SSL_set_mode(3)
    and an asynchronous capable engine is being used. An application can determine whether the engine has
    completed its processing using select() or poll() on the asynchronous wait file descriptor. This file
    descriptor is available by calling SSL_get_all_async_fds(3) or SSL_get_changed_async_fds(3). The
    TLS/SSL I/O function should be called again later. The function must be called from the same thread
    that the original call was made from.

SSL_ERROR_WANT_ASYNC_JOB

    The asynchronous job could not be started because there were no async jobs available in the pool
    (see ASYNC_init_thread(3)). This will only occur if the mode has been set to SSL_MODE_ASYNC using
    SSL_CTX_set_mode(3) or SSL_set_mode(3) and a maximum limit has been set on the async job pool through
    a call to ASYNC_init_thread(3). The application should retry the operation after a currently
    executing asynchronous operation for the current thread has completed.

SSL_ERROR_WANT_CLIENT_HELLO_CB

    The operation did not complete because an application callback set by SSL_CTX_set_client_hello_cb()
    has asked to be called again. The TLS/SSL I/O function should be called again later. Details depend
    on the application.

SSL_ERROR_SYSCALL

    Some non-recoverable, fatal I/O error occurred. The OpenSSL error queue may contain more information
    on the error. For socket I/O on Unix systems, consult errno for details. If this error occurs then no
    further I/O operations should be performed on the connection and SSL_shutdown() must not be called.

    This value can also be returned for other errors, check the error queue for details.

SSL_ERROR_SSL

    A non-recoverable, fatal error in the SSL library occurred, usually a protocol error. The OpenSSL
    error queue contains more information on the error. If this error occurs then no further I/O
    operations should be performed on the connection and SSL_shutdown() must not be called.
*/


