// Ftp Control Class -- Interfaces with Web Site to establish connection
//
// There are two interfaces between the Windows system and the web host.  There is one control class and
// this is it.  There is also a transfer class which is used to move the data.


#pragma once
#include "FtpBase.h"

class FtpTransfer;


class FtpControl : public FtpBase {

bool connected;

public:

char response[256];                       // Last Command Response

  FtpControl() : connected(false), response{0} { }
 ~FtpControl() { }

  bool   connect(Cchar* host);
  bool   setEncryp() {return webPort.setEncryp();}

  bool   initPasv(Cchar* cmd, Cchar* arg, FtpTransfer& tsfr);

  bool   sendCmd(Cchar* cmd,             char resp) {return sendCmd(cmd, 0, resp);}
  bool   sendCmd(Cchar* cmd, Cchar* arg, char resp);
  bool   sendCmd(Cchar* cmd, Cchar* arg = 0);

  bool   readResp(char c);

  String partialResp(int pos);

  bool   close();

  void   setCertCB(FtpCertCB cbf) {webPort.setCertCB(cbf);}

private:

  bool   getServerByName(Cchar* service, SockAddrIn4& sin);
  bool   inetAddr(       Cchar* p,       u_long& addr);
  bool   findHostIPaddr( Cchar* url,     u_long& addr);
  };

