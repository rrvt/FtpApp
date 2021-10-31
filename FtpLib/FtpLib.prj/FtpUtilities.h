// FtpLib Utilities -- At this point error response functions


#pragma once


void    ftpWSAErr(TCchar* src);
void    ftpSktErr(TCchar* src, int rslt);

void    ftpPutError(Cchar* err);
String& ftpGetError();
