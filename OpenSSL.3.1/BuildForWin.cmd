rem first argument is the configuration (i.e. Debug or Release)
rem second argument is path to openSSL directory base (e.g. D:\Sources\OpenSSL.3.1\)
rem third argument is "static" to indicate a static library (rather than a dll library)

rem   "C:\Program Files\JPSoft\TCMD25\tcc"  BuildForWin "%bn" "%rp" static

echo on

set Configuration=%1
set WorkSpace=%2

set OpenSSLsrc=%WorkSpace%

set OpenSSLtgt=%OpenSSLsrc%%Configuration%\

set Dirs=--prefix=%OpenSSLtgt% --openssldir=%OpenSSLtgt% --libdir=..\%Configuration%

iff "%Configuration%" == "Debug" then
  set flavor=--debug
else
  set flavor=--release
endiff

iff "%3" == "static" then
  set Static=no-shared
else
  set Static=
endiff

rem Setup Compiler system variables -- required for nmake to run

set VS2017="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
CALL %VS2017%

rem Create makefile

md /d/Ne %OpenSSLsrc%

perl Configure %flavor%  %Dirs% %Static% no-capieng VC-WIN32

nmake
nmake test
nmake install
exit
