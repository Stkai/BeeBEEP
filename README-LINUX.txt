/****************************************************************
 *                 LINUX INSTALL GUIDE                          *
 *  Latest version on: http://beebeep.sourceforge.net/help.php  *
 *                                                              *
 ****************************************************************/


1) Download the right version for your system (32 or 64 bits):
   http://beebeep.sourceforge.net/download.php

2) Uncompress the package in your favorite folder

3) From a terminal add executable permission to the application
   # chmod +x /beebeep-folder/beebeep
   
4) Check BeeBEEP dependencies and install packages if needed

   BeeBEEP 2.0 has these dependencies:
	
   libxcb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb.so.1
   libxcb-screensaver.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-screensaver.so.0
   libQt5PrintSupport.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5PrintSupport.so.5
   libQt5Multimedia.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Multimedia.so.5
   libQt5Widgets.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Widgets.so.5
   libQt5Network.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Network.so.5
   libQt5Gui.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Gui.so.5
   libQt5Core.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Core.so.5

   Where you can find packages?

   Ubuntu and Debian like
   # sudo apt-get install libxcb-screensaver0 qt5-default libqt5multimedia5 libqt5multimedia5-plugins
   
   Fedora 22 (thanks to Marcus Frohn)
   # dnf install libxcb.so.1 libxcb-screensaver.so.0 libQt5PrintSupport.so.5 libQt5Multimedia.so.5 libQt5Widgets.so.5 libQt5Network.so.5 libQt5Gui.so.5 libQt5Core.so.5 -y

   RPM packages:
   http://rpmfind.net/linux/rpm2html/search.php?query=libxcb-screensaver.so.0 
   http://rpmfind.net/linux/rpm2html/search.php?query=qt5

