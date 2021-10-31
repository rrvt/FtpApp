// Progress Bar -- While waiting for the web host to respond this provides some movement on the screen for
// the user to watch.  It indicates how much is done...


#include "stdafx.h"
#include "ProgBar.h"
#include "FtpApp.h"
#include "MainFrame.h"
#include "Resource.h"


ProgBar progBar;
CProgressCtrl ProgBar::timerCtrl;
bool          ProgBar::timerStarted = false;


// Create the progress bar, set it for n increments and start it.  Use Step to increment one increment

void ProgBar::open(int n) {
CRect rect;

  if (created) close();

  if (!mainFrm) mainFrm = theApp.mainFrm();

  getRect(rect);

  if (!ctrl.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect, mainFrm, IDC_ProgBar)) return;
  ctrl.SetMarquee(true, 10);
  ctrl.SetRange(0, n);
  ctrl.SetPos(0);
  ctrl.SetStep(1);

  created = true;
  }


void ProgBar::startTimer(int mSec) {
CRect rect;
bool  rslt;

  if (!mainFrm) mainFrm = theApp.mainFrm();

  timerID = mainFrm->SetTimer(IDC_ProgTimer, mSec, timerProc);

  getRect(rect);

  rslt = timerCtrl.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_MARQUEE, rect, mainFrm, IDC_ProgBar);

  timerStarted = timerID != 0 && rslt;
  }



void ProgBar::timerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {

  if (timerStarted) timerCtrl.StepIt();
  }


void ProgBar::stopTimer() {

  if (!timerStarted) return;

  if (timerStarted) timerCtrl.DestroyWindow();

  timerStarted = !mainFrm->KillTimer(timerID);
  }


void ProgBar::getRect(CRect& rect) {
CRect clientWndw;
int   left;
int   top;
int   width;
int   bottom;

  mainFrm->GetClientRect(clientWndw);

  width = clientWndw.right;
  left  = width / 6;
  top   = clientWndw.bottom / 2;
  bottom = top + 20;
  width = width * 2 / 3;
  rect.top    = top;
  rect.bottom = bottom;
  rect.left   = left;
  rect.right  = left + width;
  }

