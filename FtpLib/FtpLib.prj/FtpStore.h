// Ftp Store (temporary storage for data retrieved from website
// There are two distinct interfaces here (probably should have separated them)
//   * -- Capture the data from the web host in an array of FtpDatum (basically a 1024 byte buffer).
//        This is used for commands that request file data like directories, current working directory,
//        file size, file date.
//   * -- Read/Write the data from/to files.  This is used for put and get.
//
// The Iterator returns the next string.  However, the string is located in an unstructured buffer and
// needs to be parsed from the buffers.  The iterator does this.


#pragma once
#include "ExpandableP.h"
#include "FileIO.h"
#include "FtpDatum.h"
#include "IterT.h"


class FtpStore;
typedef RcdPtrT<FtpDatum> FtpDatumP;                      // Usually defined just before iterator
typedef IterT<FtpStore, FtpDatum> FtpStrIter;             // Iterator over array of Datuss


class FtpStore {

ExpandableP<FtpDatum, FtpDatumP, 2> data;

FileIO fileIO;

public:

  FtpStore() { }
 ~FtpStore() { }

  // Data Array storage operations -- used by nameList and dir operations

  void open() {clear();}

  void clear() {data.clear();}

  FtpDatum* allocate() {FtpDatum* d = data.allocate(); data += d; return d;}
  bool      isEmpty() {return nData() == 0 || data[0].p->isEmpty();}

  // File Operations -- Used by Get/Put

  bool      open(TCchar* localPath, FileIO::OpenParms mode);
  bool      writeToFile( char* blk, int n) {return fileIO.write(blk, n);}
  int       readFromFile(char* blk, int n) {return fileIO.read( blk, n);}
  void      close() {fileIO.close();}

private:

  // returns either a pointer to data (or datum) at index i in array or zero

  FtpDatum* datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  int   nData()      {return data.end();}                       // returns number of data items in array

  void  removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename FtpStrIter;
  friend class    FtpStoreIter;
  };


// An iterator over the "lines" in the data array.  Returns pointers to a line as String

class FtpStoreIter {
FtpStrIter iter;
FtpDatum*  datum;

String     line;

public:

  FtpStoreIter(FtpStore& store) : iter(store), datum(0) { }
 ~FtpStoreIter() { }

  String* operator() ();
  String* operator++ (int);

private:

  String* getString();

  bool    initDatum(FtpDatum* datm) {datum = datm; if (datum) datum->initRead();  return datum;}

  FtpStoreIter() : iter(*(FtpStore*)0) { }
  };
