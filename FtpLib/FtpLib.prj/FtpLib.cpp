// Ftp Library -- Main Interface
// The Application should include this where it needs access to the web host.


#include "stdafx.h"
#include "FtpLib.h"
#include "filename.h"
#include "FtpTransfer.h"
#include "FtpUtilities.h"
#include "NotePad.h"

static const TCchar* Prefix = _T("   <FtpLib3> ");


// FtpLib Constructor
// WSAStartup initialization done once -- might as well be done here
// Start the OpenSSL library here too

FtpLib::FtpLib() {
WSADATA wsa;
int     err = WSAStartup(MAKEWORD(2, 2), &wsa);                   // Required in Windows

  if (err) {ftpSktErr(_T("WSAStartup() failed"), err);}

  SSL_library_init();
  }


// login - log in to remote server

bool FtpLib::login(Cchar* user, Cchar* pass) {

  if (!sendCmd("USER", user, '3')) return control.response[0] == '2';

  return sendCmd("PASS", pass, '2');
  }



// FtpSysType - send a SYST command, return false if the command fails, otherwise
// returns true and sets systemType with the response from the web host.

bool FtpLib::sysType(String& systemType) {

  if (!sendCmd("SYST", '2')) return false;

  systemType = control.partialResp(4);   return true;
  }


// Same as above, but copies the data in the buffer as Ansi characters

bool FtpLib::sysType(char* buf, int lng) {

  if (!sendCmd("SYST", '2')) return false;

  strcpy_s(buf, lng, control.response + 4);   return true;
  }


// return true and the current remote directory or false if it fails

bool FtpLib::pwd(String& dir) {
int pos;
int pos2;

  if (!sendCmd("PWD", '2')) return false;

  dir = control.partialResp(4);

  pos = dir.find(_T('"')) + 1;    pos2 = dir.find(_T('"'), pos);     dir = dir.substr(pos, pos2 - pos);

  return true;
  }


// pwd - get current remote directory and return in a buffer as Ansi characters

bool FtpLib::pwd(char* buf, int max) {
int   n = max;
char* b = buf;
char* s;

  if (!sendCmd("PWD", '2')) return 0;

  s = strchr(control.response, '"');   if (!s) return false;

  s++;

  while (--n && *s && *s != '"') *b++ = *s++;     *b = '\0';    return true;
  }


// Returns true when the Unix like directory name list (each line contains on directory name in the
// current directory.  The data is stored in the data store which can be read later.

bool FtpLib::nameList(Cchar* webPath, FtpStore& ftpStore) {
FtpTransfer tsfr;

  if (!sendCmd("TYPE A", '2')) return false;

  if (!control.initPasv("NLST", webPath, tsfr)) return false;

  ftpStore.open();   tsfr.webDataToStore(ftpStore);   tsfr.close();

  return control.readResp('2');
  }


// Returns true when the Unix like directory list (each line contains a line for each entity in the
// current directory.  The data is stored in the data store which can be read later.

bool FtpLib::dir(Cchar* webPath, FtpStore& ftpStore) {
FtpTransfer tsfr;

  ftpStore.clear();

  if (!sendCmd("TYPE A", '2')) return false;

  if (!control.initPasv("LIST -a -l", webPath, tsfr)) return false;

  ftpStore.open();   tsfr.webDataToStore(ftpStore);    tsfr.close();

  return control.readResp('2');
  }


// FtpChdir - change path at remote

bool FtpLib::chDir(Cchar* webPath) {return sendCmd("CWD", webPath, '2');}


// FtpCDUp - move to parent directory at remote

bool FtpLib::cdUp() {return sendCmd("CDUP", '2');}


// FtpRmdir - remove directory at remote

bool FtpLib::rmDir(Cchar* webPath) {return sendCmd("RMD", webPath, '2');}


// Make a new directory in the web host, returns true when the directory has been created

bool FtpLib::mkDir(Cchar* webPath) {return sendCmd("MKD", webPath, '2');}


// Return true when the size of the file is found, else return false

bool FtpLib::size(Cchar* webPath, int& size) {
int  rsp;

  if (!sendCmd("SIZE", webPath, '2')) return false;

  return sscanf_s(control.response, "%d %d", &rsp, &size) == 2;
  }


// return true when the date of the file is found, else return false

bool FtpLib::modDate(Cchar* webPath, Date& date) {

  if (!sendCmd("MDTM", webPath, '2')) return false;

  ToUniCode uni(control.response+4);

  try {ToDate    to(uni());    date = to();}
    catch (...) {notePad << Prefix << _T("Date Interprets: ") << uni() << nCrlf;
    }

  return true;
  }


// Return true when the file is renamed succeeds, esle return false

bool FtpLib::ren(Cchar* webSrc, Cchar* webDst) {

  if (!sendCmd("RNFR", webSrc, '3')) return false;

  return sendCmd("RNTO", webDst, '2');
  }


// Return true when file is deleted, else return false

bool FtpLib::del(Cchar* webPath) {return sendCmd("DELE", webPath, '2');}


// return true if the file is copied from the web host to the local path with the same name and the same
// position in the local directories.

bool FtpLib::get(Cchar* webPath, TCchar* localPath, FtpStore& ftpStore) {
FtpTransfer tsfr;
int         mode = FileIO::Write | FileIO::Create;

  if (!sendCmd("TYPE", isAsciiFile(webPath) ? "A" : "I", '2')) return false;

  if (!control.initPasv("RETR", webPath, tsfr)) return false;

  if (!ftpStore.open(localPath, (FileIO::OpenParms) mode)) return false;

  tsfr.webDataToFile(ftpStore);

  ftpStore.close();    tsfr.close();   return control.readResp('2');
  }


// return true if the file is copied from the local file to the web host with the same name and the same
// position in the local directories.

bool FtpLib::put(TCchar* localPath, Cchar* webPath, FtpStore& ftpStore) {
FtpTransfer tsfr;
bool        rslt;

  if (!sendCmd("TYPE", isAsciiFile(webPath) ? "A" : "I", '2')) return false;

  if (!ftpStore.open(localPath, FileIO::Read)) return false;

  if (!control.initPasv("STOR", webPath, tsfr)) {ftpStore.close(); return false;}

  rslt = tsfr.fileDataToWeb(ftpStore);

  ftpStore.close();   tsfr.close();   return control.readResp('2');
  }


static TCchar* asciiExt[] = {_T("htm"), _T("html"), _T("css")};


// Ascii files are copied as text, the rest are copied as images

bool FtpLib::isAsciiFile(Cchar* webFile) {
ToUniCode uni(webFile);
String    ext = getExtension(uni());
int       i;
int       n = noElements(asciiExt);

  for (i = 0; i < n; i++) if (ext == asciiExt[i]) return true;

  return false;
  }



