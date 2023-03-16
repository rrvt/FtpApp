// Report Base


#pragma once
#include "Date.h"
#include "Printer.h"

class CScrView;
class DevBase;
class NotePad;



class ReportBase {
protected:

NotePad& np;

String   title;
int      maxPages;
Date     dateModified;

bool     printing;

public:
PrtrOrient prtrOrietn;

               ReportBase(NotePad& notePad) : np(notePad), maxPages(0),
                                                             printing(false), prtrOrietn(PortOrient) { }
              ~ReportBase() { }

          void setTitle(TCchar* t) {title = t;}

  virtual void display(CScrView& vw);

  virtual void dspHeader(DevBase& dev, int pageNo = 1) { }
  virtual void dspFooter(DevBase& dev, int pageNo = 1) { }

  virtual void prtHeader(DevBase& dev, int pageNo);
  virtual void prtFooter(DevBase& dev, int pageNo);

protected:

          void getPageAttr(CScrView& vw);

  virtual void getData(CScrView& vw) = 0;

private:

  ReportBase() : np(*(NotePad*)0) { }
  };


