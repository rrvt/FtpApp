// Text Operations


#pragma once
#include "ClipLine.h"
#include "Horiz.h"
#include "Wrap.h"

class  TxtOut;
class  DevCtx;
class  DevTabs;
class  VertMgmt;
struct NoteNmbr;


// Specifies what to do after wrap completes

enum AfterTxt {NilAftr,    FaceAftr,      SizeAftr,      BoldAftr,
               ItalicAftr, UnderLineAftr, StrikeOutAftr, PopAftr,
               clrTbsAftr, CRAftr,        LeftAftr,      TabAftr,
               CenterAftr, RightAftr,     BegULAfter,    NmbrAftr,
               EndULAftr,  DoCrlfAftr,    DoEndPgAftr
               };


struct TxtOps {
TxtOut&   txtOut;
DevCtx&   dvx;

Horiz&    horz;                               // Manage Horizontal movement in window or page
DevTabs&  devTabs;
VertMgmt& vert;                               // Manage vertical movement in window or page

AfterTxt  afterTxt;                           // Operation to perform after text wrap concludes
double    afterVal;                           // The value to be used by the ater operation
String    fontFace;                           // Font Face when afterTxt chooses this option

bool&     center;                             // center string horizonally in window when true;
bool&     right;                              // right align in window when true;

double    pos;                                // position each line uses during wrap operation
Wrap      wrap;

  TxtOps() : txtOut(*(TxtOut*)0), dvx(*(DevCtx*)0), horz(*(Horiz*)0),
             devTabs(*(DevTabs*)0), vert(*(VertMgmt*)0),
             center(*(bool*)0), right(*(bool*)0), wrap(dvx) { }

  TxtOps(TxtOut& to, AfterTxt aftr, double aftrVal, TCchar* face);

 ~TxtOps();

  void clear();

  void setPosition(String& sum);

  void start(bool wrapEnabled);

  bool operator() (String& sum) {pos = horz.pos();   return output(sum);}
  void cleanupWrap(String& sum);

  void afterTxtOut();

private:

  bool output(String& sum);
  void fragmentOut(String& frag);
  void clipTabInvert();
  void setMetric();

  void tab() {clipLine.setHzPos(horz.currentPos()); clipLine.close(dvx);}
  void findNextTab() {devTabs.findNextTab(horz);  horz.position = devTabs.cur->pos;}

  void evalNmbr(NoteNmbr& nmbr);
  };



