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

The first task the user must perform is to create a database containing all the files on the local
and remote web site directories.  Then at some later time (could be seconds or months) the user
must run a command to create a current list of files on the local and remote web site directories.
After the current list is constructed it is compared with the older version.  Any file that has changed
or is not on the web site but is on the local site is placed in a third list.  Any files in a
remote web site directory and not on the local web site directory is also placed on this third list.
An indicator accompanies each file on the difference list (this third list) which defines why the
file is on the list.  Finally the user may select files to update (or select update all the files).

Now how would the appication know that a change has occurred.  Since the older list contains the size
and date of the each file, the new file list can compare the date and size of each file on the local
site.  Any changes then are obvious.  Please note that the remote sizes and dates are not reliable
indicators of change as different operating systems require different end of line sequences which make
the sizes different.  Also the dates the files are created on the remote and local sites are by
definition different and the OS on the remote site can change them at any time (e.g. recovery, movement).

Furthermore, since both lists include all files on both sites it is possible to discover new files and
old files.  If a file exists in the local site in the older list and doesn't exist in the current list
but still exists on the remote site then it has probably been deleted for the local site and should be
deleted from the remote site.

In any event this application works more like Dreamweaver in that it alerts the user to files that
need updating on the web site and facilitating the update.

## Updates

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

## Getting Started

This version was created with Visual Studion 2017.  It was compiled with the following properties:

  o Windows SDK Version: 10.0.18362.0
  o Platform Toolset: visual Studio 2017 (v141)
  o MFC: Use MFC in a Shared DLL
  o Character Set:  Use Unicode Character Set
  o Precompiled Header:  Not Using Precompiled Headers
  o Linker/Additional Dependencies:  Htmlhelp.lib, OpenSSL.3.1\libssl.lib, OpenSSL.3.1\libcrypto.lib

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

I've included the OpenSSL.3.1 library to make sure that the application can be built.  Having said that,
it is compiled only for the libraries that are needed by the Ftp Library.  It compiles into two libraries
that are referenced in FtpApp.

Compiling OpenSSL is a big, complex process best left to the originators of OpenSSL.  So there is a
command file (i.e. BuildForWin.cmd) that runs in JP Software's Take Command (i.e. a third party
DOS box) that will build the OpenSSL libraries,
test files, OpenSSL, and other miscellaneous features.  The command file, BuildForWin.cmd, also
initializes the nMake facility for Visual Studio.  BuildForWin.cmd accepts three arguments:

  * The Configuration (Debug or Release)
  * The path to the files (e.g. D:\Source\OpenSSL.3.1\)
  * The word "static" to indicate static libraries.

Since it takes a long time to run this command file, it is not compiled when the other projects are
compiled.

## Prerequisites

The WiX Toolset must be installed in Visual Studio.  The "HTML Help Workshop" (google it) must be
installed.  Visual Studio 2017 or later.  OpenSSL 3.1 or later.

### Installing

The /Release/FtpAppInstaller.msi will install the product.

## Authors

Robert R. Van Tuyl

Contact:  romeo romeo victor tango alpha tango sam whisky delta echo dot charlie oscar mike

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details


