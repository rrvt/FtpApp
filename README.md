# FtpApp -- An application for updating encrypted login web sites with ftp protocols

First let me point out that this application works on Yahoo Small Business Web Host.  I do not
have the resources to test it at many, several or even two different web hosts.

Web Hosts are getting a bit smarter about login protocols for updating web sites.  An old version of
Dreamweaver stop updating my web site because they are now requiring login to the site to be encrypted.
However, just because the login is encrypted the rest of the transactions to update a web site are
just plain old ftp.

This app uses OpenSSL to perform the encrypted part of the update process and then the ftp is allowed
to go forward.

When this first happened I turned to OTS products to update my web site.  FileZilla Pro seemed to work
and I am able to update the web site with it.  However it requires the user to figure out which files
need updating.  Filezilla helps by highlighting various files it deems might require updating but there
a lot of false positives.  FtpApp attempts to make this job easier.

The first task the user must perform is to create a database containing all the files on web site.
The goal is to get a list of all the files that are needed by the web site host with size and date
attributes that are consistent with the local version of the web site.  Unfortunately, many web hosts
use Unix/Linix server and the file sizes are different for text files (Linix does not require a
line feed to help define lines whereas Windows does).  Furthermore dates at a web host are not
necessarily consisten with either the uploading of the file or the dates on the local copy of a file.

The dilema is resolved in FtpApp by saving the local file attributes after a file is uploaded to the web
host.  Thus the next time changes are made to the local files a comparison may be made with the
assurance that only changed files will be noted.  Comparison is made using the files size and
modification date.

However, there is a little issue at the beginning.  If the web site already exists and FtpApp is
being used for the first time to update the web site then there is not past history database for
the web site.  In this case, the web host is contacted and all the web site files are downloaded and
placed in the "Previous" version of the web site.  Since the dates and sizes are probably all
different for the previous and current local site, they will all need to be uploaded so that
the web host and local files are consistent and the "previous" files list is updated with the local
files date and size.  After the initial upload FtpApp will only indicate changed fiels for uploading.

And there is one other case.  Suppose one does not have a local copy of the web site.  There is a
command to download all the files from the web host.  This will establish a local copy and save the
date and size of the local copy of the files in the "Previous" database for the web site.

Furthermore, since both lists include all files on both sites it is possible to discover new files and
old files.  If a file exists in the local site in the older list and doesn't exist in the current list
but still exists on the remote site then it has probably been deleted for the local site and should be
deleted from the remote site.

It should also be noted that changes suggested by FtpApp must be confirmed by checking a box for each
file for which an action is suggested (or selecting the "Check All" button).

In any event this application works more like Dreamweaver in that it alerts the user to files that
need updating on the web site and facilitating the update.

## Getting Started

This version was created with Visual Studion 2022.  It was compiled with the following properties:

  o Windows SDK Version: Latest Win10 SDK
  o Platform Toolset: visual Studio 2022 (v141)
  o MFC: Use MFC in a Shared DLL
  o Character Set:  Use Unicode Character Set
  o Precompiled Header:  Not Using Precompiled Headers
  o Linker/Additional Dependencies:  Htmlhelp.lib, OpenSSL.3.1\libssl.lib, OpenSSL.3.1\libcrypto.lib
  o OpenSSL.3.1 compiled separately for both debug and release.  Details below...

The HTML Help Workshop (HHW), Version 4.74.8702.0 was used to prepare the help file (WixApp.chm).  It is
copied into the Release directory.  I used Dreamweaver (DW) to do most of the content production of the
help files that the HTML Help Workshop produces (i.e. HHW is used to produce the pages/files needed
and DW is used to fill in the content).

Additionally the WIX Installer, The very old Help Workshop and SlickEdit were used to process the code.
The Product (Wix installation file) was produced by my WixApp but it can be manipulated by
hand for changes.

### FtpLib

This version of a FtpLib was based on ideas first published by Thomas Pfau and modified by
magnus kulke.  I've reorganized those ideas into multiple classes with different concerns.

### OpenSSL

I've used OpenSSL.3.1 in the build.  it is compiled only for the libraries that are needed by the
Ftp Library.  It compiles into two libraries that are referenced in FtpApp.

Compiling OpenSSL is a big, complex process best left to the originators of OpenSSL.  So there is a
command/script file (i.e. BuildForWin.cmd) that will build the OpenSSL libraries, test files,
OpenSSL, and other miscellaneous features.  The command file, BuildForWin.cmd, also
initializes the nMake facility for Visual Studio.  BuildForWin.cmd accepts three arguments:

  * The Configuration (Debug or Release)
  * The path to the files (e.g. D:\Source\OpenSSL.3.1\)
  * The word "static" to indicate static libraries.

Since it takes a long time to run this command file, it is not compiled when the other projects are
compiled.  Here is the command/script file:

```
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
```

The command/script is run in jpsoftware's Take Command (i.e. tcc). Their websit is https://jpsoft.com/.

```
tcc  BuildForWin Release D:\Sources\OpenSSL.3.1\ static
```


## Prerequisites

The WiX Toolset must be installed in Visual Studio.  The "HTML Help Workshop" (google it) must be
installed.  Visual Studio 2017 or later.  OpenSSL 3.1 or later.

### Installing

The /Release/FtpAppInstaller.msi will install the product.

## Updates

### Update 3/31/23

Add Report Orientation Dialog.

### Update 3/15/23

Complete move of orientation to reports.

### Update 3/8/23

Added numbers to notepad archive output.   Fixed (hopefully) Print and Print Preview.  See MakeApp for
details.

### Update 10/28/22

Start app in the same position and size as when it exited the last time.  Update the background of the
toolbar.

### Update 6/12/22

Complete rewrite of the FtpLib obtained off the web.  One issue was the lack of a user molification
output during long running sequences.  So the long running sequences were put into a "Worker Thread"
and the function then sent an update to a "Progress Bar" in the UI Thread (the main Thread).  In one
case the file that was transferred is also displayed in the main window just after the transfer takes
place (with the usual caviot that Windows messages don't happen instantly).

### Update 01/31/22

Updates to the library, mostly about the toolbar, but also NewAllocator, and a minor addition to
Expandable.

Also scrubbed the project files to remove all references to precompiled header files which I turned off
in VS17.  Unfortnately VS does not remove all references and pch files kept cropping up.  Not only does
that slow down the compiler but the files are large...

### Update 12/18/21

While creating a template for a dialog box only application with a toolbar the Library was added to and
changed to hopefully simplify toolbar creation.  This version of the toolbar includes buttons, edit boxes,
combo boxes and drop down menus as well as the traditional 16x15 icons.

FtpApp is a tradional App, has a traditional 16x15 pixel icon toolbar.

### Update 12/18/21

While creating a template for a dialog box only application with a toolbar the Library was added to and
changed to hopefully simplify toolbar creation.  This version of the toolbar includes buttons, edit boxes,
combo boxes and drop down menus as well as the traditional 16x15 icons.

FtpApp is a tradional App, has a traditional 16x15 pixel icon toolbar.

### Update 11/13/21

The changes are in the library:

  - Expandable and ExpandableP -- moved the functions out of the class to make the compilation smaller,
also fixed one bug (or feature) in the ExpandableP insert sorted function that would throw out entries
that were already in in the array.
  - FileIO -- Changed the type of path from "String&" to "TCchar*".  It turns out that automatic
conversion from String to TCchar* is easy, but the reverse is is not easy.  Also added a function to
get the length of a file given a path.
  - FineName -- Added a function to get the directory name of from a path
  - NewAllocator -- Corrected the debug dependency from if to ifdef
  - ToolBar -- Added data for drop down menus

### Update 10/6//21 Later

Added some logic to FtpLib recover a pointer to the certificate in the Application.  Don't have a clue
what one could do with that information.

### Update 10/6/21

The program was doing something useful, so I spent a few hours trying to figure out to get it in GitHub.
The problems was mostly about very large files, how to find them and delete them in OpenSSL.

But, ...  This was a program in development.  I finally took the time to add comments here and there.
The last update I had gone through the application project.  This update I've looked at the FtpLib
project very carefully.  Yuk...  There was a lot of extra code in the modules that was there specifically
to do debugging.  There was code that was needed to formulate ideas but was now clearly unimportant.
There was code that was replicated all over the place and could be replaced with a simple change
somewhere.  And a few comments were added here and there.

### 10/4/21

Added some comments, removed unused code, general cleanup...

## Authors

Robert R. Van Tuyl

Contact:  romeo romeo victor tango alpha tango sam whisky delta echo dot charlie oscar mike

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details


