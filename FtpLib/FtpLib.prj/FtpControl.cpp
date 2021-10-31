// Ftp Control Class -- Interfaces with Web Site to establish connection


#include "stdafx.h"
#include "FtpControl.h"
#include "FtpCmdLog.h"
#include "FtpTransfer.h"
#include "FtpUtilities.h"
#include <ws2tcpip.h>


// Connect - connect to remote server, return true if connected

bool FtpControl::connect(Cchar* host) {
Socket      skt;
SockAddrIn4 sin;
int         on = 1;
char        lhost[1024];
char*       pos;
int         rslt;

  memset(&sin, 0, sizeof(sin));

  sin.sin_family = AF_INET;

  strcpy_s(lhost, sizeof(lhost), host);    pos = strchr(lhost, ':');

  if (!pos && !getServerByName("ftp", sin)) return false;

  else {
    *pos++ = '\0';

    if (isdigit(*pos)) sin.sin_port = htons(atoi(pos));
    else if (!getServerByName("ftp", sin)) return false;
    }

  if (!inetAddr(lhost, sin.sin_addr.s_addr) && !findHostIPaddr(lhost, sin.sin_addr.s_addr)) return false;

  skt = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (skt == INVALID_SOCKET) {ftpWSAErr(_T("::socket")); return false;}

  rslt = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (Cchar*) &on, sizeof(on));
  if (rslt ==  SOCKET_ERROR ) {ftpWSAErr(_T("setsockopt")); closesocket(skt); return false;}

  if (::connect(skt, (SockAddr*) &sin, sizeof(sin)))
                                      {ftpWSAErr(_T("connect")); closesocket(skt); return false;}
  connected = true;

  webPort.initEncryp(skt);

  if (!readResp('2')) {close(); return false;}

  if (!sendCmd("AUTH", "TLS", '2')) return false;

  return setEncryp();
  }


bool FtpControl::getServerByName(Cchar* service, SockAddrIn4& sin) {
ServerEnt* pse = getservbyname(service, "tcp");

  if (!pse) {ftpWSAErr(_T("getservbyname"));  return false;}

  sin.sin_port = pse->s_port;    return true;
  }


typedef struct in_addr      InAddr;


bool FtpControl::inetAddr(Cchar* p, u_long& addr) {
InAddr inAddr;

  if (inet_pton(AF_INET, p, &inAddr) <= 0)  return false;

  addr = inAddr.S_un.S_addr;    return true;
  }


typedef struct addrinfo     AddrInfo;
typedef short               AddrFamily;
typedef struct sockaddr_in6 SockAddrIn6;


bool FtpControl::findHostIPaddr(Cchar* url, u_long& addr) {
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

  if (rslt) {ftpSktErr(_T("findHostIPaddr"), rslt);  return false;}

  stringBuf[0] = 0;  family = info->ai_family;

  switch (family) {
    case AF_UNSPEC: ftpPutError("Family Unspecified");   return false;

    case AF_INET  : sockaddr_ipv4 = (SockAddrIn4*) info->ai_addr;

                    addr = sockaddr_ipv4->sin_addr.s_addr; break;

    case AF_INET6 : sockaddr_ipv6 = (SockAddrIn6*) info->ai_addr;

                    sockaddr_ipv6->sin6_addr;

                    ftpPutError("Family: IP6 address");  return false;

                    break;

    default       : ftpPutError("Gsmily: Other format"); return false;
    }

  return true;
  }


bool FtpControl::initPasv(Cchar* cmd, Cchar* arg, FtpTransfer& tsfr) {
Sin           sin;
char*         cp;
unsigned int  v_i[6];
unsigned char v[6];

  memset(&sin, 0, sizeof(sin));

  sin.in.sin_family = AF_INET;

  if (!sendCmd("PASV", '2')) return false;

  cp = strchr(response,'(');    if (cp == NULL) return false;

  cp++;

  sscanf_s(cp, "%u,%u,%u,%u,%u,%u", &v_i[2], &v_i[3], &v_i[4], &v_i[5], &v_i[0], &v_i[1]);

  for (int i = 0; i < 6; i++) v[i] = (unsigned char) v_i[i];

  if (correctPasv) if (!webPort.correctPasvResponse(v)) return false;

  sin.sa.sa_data[2] = v[2];
  sin.sa.sa_data[3] = v[3];
  sin.sa.sa_data[4] = v[4];
  sin.sa.sa_data[5] = v[5];
  sin.sa.sa_data[0] = v[0];
  sin.sa.sa_data[1] = v[1];

  if (offset != 0) {
    char buf[16];

    sprintf_s(buf, sizeof(buf), "%ld",  offset);

    if (!sendCmd("REST", buf, '3')) return false;
    }

  if (!tsfr.webPort.initDataXsfr()) return false;

  if (!sendCmd(cmd, arg)) {ftpPutError("write");  return false;}

  if (!tsfr.webPort.connect(sin)) return false;

  return readResp('1');
  }


// Return response from pos to end as Unicode Characters

String FtpControl::partialResp(int pos) {
ToUniCode uni(response + pos);
String    s = uni();  return s;
  }



bool FtpControl::close() {

  if (!connected) return false;

  sendCmd("QUIT", '2');

  ftpCmdLog.stopLogging();    webPort.close();   connected = false;   return true;
  }


// Send a command and test response:  The first character of the response must be the character resp
// Fails when command not set or response charcter is not found

bool FtpControl::sendCmd(Cchar* cmd, Cchar* arg, char resp) {

  if (!sendCmd(cmd, arg)) return false;

  return readResp(resp);
  }


// The command and arguments are combined (provided the arg pointer is not zero) and the command is sent
// Clears the response buffer and returns result of send the command

bool FtpControl::sendCmd(Cchar* cmd, Cchar* arg) {
char cmdBuf[256];

  strcpy_s(cmdBuf, sizeof(cmdBuf), cmd);

  if (arg) {strcat_s(cmdBuf, sizeof(cmdBuf), " ");   strcat_s(cmdBuf, sizeof(cmdBuf), arg);}

  ftpCmdLog(cmdBuf, true);

  strcat_s(cmdBuf, sizeof(cmdBuf), "\r\n");

  *response = 0;   return webPort.write(cmdBuf, strlen(cmdBuf));
  }


// read a response from the server, logs it and checks the first character against the character c
// Succeeds when first character is equal to c

bool FtpControl::readResp(char c) {
char match[5];

  webPort.initResponse();

  if (!webPort.readLine(response, sizeof(response))) {ftpPutError("Read Response"); return false;}

  ftpCmdLog(response, false);

  if (response[3] == '-') {

    strncpy_s(match, sizeof(match), response, 3);   match[3] = ' ';   match[4] = '\0';

    do {
      if (!webPort.readLine(response, sizeof(response))) break;

      ftpCmdLog(response, false);

      } while (strncmp(response, match, 4));
    }

  return *response == c;
  }

