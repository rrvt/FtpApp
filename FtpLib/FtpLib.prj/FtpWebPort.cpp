// Manage buffered reads and writes


#include "stdafx.h"
#include "FtpWebPort.h"


// Returns one line of the response to a command

bool FtpWebPort::readLine(char* blk, int noBytes) {
int  i;
int  n = noBytes-1;
char ch;

  for (i = 0, ch = get(); i < n && ch; i++, ch = get()) {*blk++ = ch;  if (ch == '\n') break;}

  *blk = 0;   return i > 0;
  }


// Return the next character from the buffer (and/or read another buffer)
// Returns zero when no more characters are read from the buffers

char FtpWebPort::get() {
char ch;

  while (!datum.getChar(ch)) if (!fillBuf()) return 0;

  return ch;
  }


// Response tend to be short, so we are using a static buffer
// Returns true when not empty

bool FtpWebPort::fillBuf() {datum.set(webIO.read(datum));  return !datum.isEmpty();}



// dir and nameList require extensive buffering, so we turn to the FtpStore.  Everything in the store
// is intialized when this function is called.
// Allocate a datum node and read the response data into the node, then initialize the datum for reading
// Returns true when the datum is full (which indicates more data may be available to be read).

bool FtpWebPort::webDataToStore(FtpStore& ftpStore) {
FtpDatum* datum = ftpStore.allocate();
int       n = webIO.read(*datum);

  return datum->set(n);
  }


// Read a block from the host and store it in a file, note ftpStore is already for the file operations
// Returns true if transfer is successful, false if not data is transferred.

bool FtpWebPort::webDataToFile(FtpStore& ftpStore) {
int n = webIO.read(datum);   if (!n) return false;

  return ftpStore.writeToFile(datum.buf(), n);
  }


// Read data from a file into a single buffer and then write it to the host.
// Returns true if data is transferred, else false

bool FtpWebPort::fileDataToWeb(FtpStore& ftpStore) {
char* buf  = datum.buf();
int   size = datum.size();
int   n    = ftpStore.readFromFile(buf, size);   if (!n) return false;

  return webIO.write(buf, n);
  }

