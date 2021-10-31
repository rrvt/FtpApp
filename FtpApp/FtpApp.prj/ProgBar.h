// Progress Bar


#pragma once

class MainFrame;

typedef void CALLBACK TimerProc(HWND hWnd,            // handle of CWnd that called SetTimer
                                UINT nMsg,            // WM_TIMER
                                UINT_PTR nIDEvent,    // timer identification
                                DWORD dwTime          // system time
                                );


// There are three operations that seem to work well:
//    * -- open
//    * -- step
//    * -- close
//
// There is one static control that is supplied here: progBar

class ProgBar {

MainFrame*    mainFrm;

CProgressCtrl ctrl;
bool          created;

static CProgressCtrl timerCtrl;
static bool          timerStarted;
int                  timerID;

public:

  ProgBar() : mainFrm(0), created(false), timerID(0) { }
 ~ProgBar() {close();}

  void open(int n);

  void step() {if (created) ctrl.StepIt();}

  void close() {if (created) ctrl.DestroyWindow();  created = false;}

  void startTimer(int mSec);

  static TimerProc timerProc;

  void stopTimer();

private:

  void getRect(CRect& rect);
  };


extern ProgBar progBar;
