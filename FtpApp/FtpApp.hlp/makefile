

FtpApp.chm : *.htm FtpApp.hhp FtpApp.hhc App.css
  -1 "C:\Program Files (x86)\HTML Help Workshop\hhc" FtpApp


Clean.cln    :
  @echo "Cleaning"
  cd $(MAKEDIR)
  del /F FtpApp.chm


Release/FtpApp.chm : FtpApp.chm
  copy $(MAKEDIR)\FtpApp.chm  $(MAKEDIR)\..\Release\FtpApp.chm


Release/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Release
  del FtpApp.chm
  cd $(MAKEDIR)


Debug/FtpApp.chm : FtpApp.chm
  copy $(MAKEDIR)\FtpApp.chm  $(MAKEDIR)\..\Debug\FtpApp.chm


Debug/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Debug
  del FtpApp.chm
  cd $(MAKEDIR)


Release/Rebuild.rbd : Release/Clean.cln Release/FtpApp.chm


Debug/Rebuild.rbd   : Debug/Clean.cln Debug/FtpApp.chm


