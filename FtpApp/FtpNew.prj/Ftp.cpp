

#include "stdafx.h"
#include "Ftp.h"
#include "FileName.h"
#include "FtpErr.h"
#include "FtpTransport.h"
#include "NotePad.h"


Ftp ftp;


Ftp::Ftp() {
WSADATA wsa;
int     err = WSAStartup(MAKEWORD(2, 2), &wsa);                   // Required in Windows

  if (err) {ftpErr.skt(_T("WSAStartup() failed"), err);}
  }


Ftp::~Ftp() {WSACleanup();}


bool Ftp::login(TCchar* userId, TCchar* password) {
//ToAnsi uid(userId);
//ToAnsi pwd(password);

  if (!sendCmd(_T("USER"), userId) || !readRsp('3')) return lastCh == _T('2');

  return sendCmd(_T("PASS"), password) && readRsp('2');
  }


// return true and the current remote directory or false if it fails

bool Ftp::pwd(String& dir) {
int pos;
int pos2;

  if (!sendCmd(_T("PWD")) || !readRsp('2')) return false;

  pos = lastResp.find(_T('"')) + 1;    pos2 = lastResp.find(_T('"'), pos);

  dir = lastResp.substr(pos, pos2 - pos);   return true;
  }




// Returns true when the Unix like directory list (each line contains a line for each entity in the
// current directory.  The data is stored in the data store which can be read later.

bool Ftp::dir(TCchar* path) {

  if (!sendCmd(_T("TYPE A")) || !readRsp('2')) return false;

  if (!initPassiveMode(_T("LIST -a -l"), path)) return false;

  transport.read();   transport.closeSkt();

  return readRsp('2');
  }


// Copy transport store to web host file at webPath

bool Ftp::put(TCchar* webPath) {

  if (!sendCmd(_T("TYPE"), isAsciiFile(webPath) ? _T("A") : _T("I")) || !readRsp('2')) return false;

  if (!initPassiveMode(_T("STOR"), webPath)) return false;

  transport.write();   transport.closeSkt();   transport.clear();

  return readRsp('2');
  }


// copy file from web host to transport buffer

bool Ftp::get(TCchar* webPath) {
int mode = FileIO::Write | FileIO::Create;

  if (!sendCmd(_T("TYPE"), isAsciiFile(webPath) ? _T("A") : _T("I")) || !readRsp('2')) return false;

  if (!initPassiveMode(_T("RETR"), webPath)) return false;

  transport.clear();   transport.read();   transport.closeSkt();

  return readRsp('2');
  }


// Return true when file is deleted, else return false

bool Ftp::del(TCchar* webPath) {return sendCmd(_T("DELE"), webPath) && readRsp('2');}


bool Ftp::initPassiveMode(TCchar* cmd, TCchar* arg) {
int     pos;
String  s;
TCchar* cp;
int     v[6];
Sin     sin;

  if (!sendCmd(_T("PASV")) || !readRsp('2')) return false;

  pos = lastResp.find('(');    if (pos < 0) return false;

  s = lastResp.substr(pos+1);  cp = s.str();

  _stscanf_s(cp, _T("%u,%u,%u,%u,%u,%u"), &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);

  ZeroMemory(&sin, sizeof(sin) );
  sin.in.sin_family = AF_INET;

  sin.sa.sa_data[2] = char(v[2]);
  sin.sa.sa_data[3] = char(v[3]);
  sin.sa.sa_data[4] = char(v[4]);
  sin.sa.sa_data[5] = char(v[5]);
  sin.sa.sa_data[0] = char(v[0]);
  sin.sa.sa_data[1] = char(v[1]);

  if (!sendCmd(cmd, arg)) {ftpErr.put(_T("Passive Transport Command Failure"));  return false;}

  if (!transport.open(sin)) {ftpErr.put(_T("Passive Transport Connection Failed"));   return false;}

  return readRsp('1');
  }



// Return true when the size of the file is found, else return false

bool Ftp::size(TCchar* path, int& size) {
int  code;
uint pos;

  if (!sendCmd(_T("SIZE"), path) || !readRsp('2')) return false;

  code = lastResp.stoi(pos);  if (code != 213 || lastResp[pos] != _T(' ')) return false;

  size = lastResp.substr(pos+1).stoi(pos);   return true;
  }


// return true when the date of the file is found, else return false

bool Ftp::date(TCchar* path, Date& val) {
int  code;
uint pos;

  if (!sendCmd(_T("MDTM"), path) || !readRsp('2')) return false;

  code = lastResp.stoi(pos);  if (code != 213 || lastResp[pos] != _T(' ')) return false;

  try {ToDate to(lastResp.substr(pos+1));    val = to();}
    catch (...) {notePad << _T("Failed Date Interpretation: ") << lastResp << nCrlf;
    }

  return true;
  }




static TCchar* asciiExt[] = {_T("htm"), _T("html"), _T("css")};


// Ascii files are copied as text, the rest are copied as images

bool Ftp::isAsciiFile(TCchar* webFile) {
String    ext = getExtension(webFile);
int       i;
int       n = noElements(asciiExt);

  for (i = 0; i < n; i++) if (ext == asciiExt[i]) return true;

  return false;
  }



void Ftp::close() {
  FtpStore::close();   transport.close();
  }



#if 1
#else
FtpTransfer tsfr;

  ftpStore.clear();

  if (!control.initPasv("LIST -a -l", webPath, tsfr)) return false;

  ftpStore.open();   tsfr.webDataToStore(ftpStore);    tsfr.close();

  return control.readRsp('2');
#endif

