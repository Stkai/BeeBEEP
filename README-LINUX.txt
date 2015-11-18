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

   BeeBEEP 2.0
	
   libxcb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb.so.1
   libxcb-screensaver.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-screensaver.so.0
   libQt5PrintSupport.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5PrintSupport.so.5
   libQt5Multimedia.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Multimedia.so.5
   libQt5Widgets.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Widgets.so.5
   libQt5Network.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Network.so.5
   libQt5Gui.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Gui.so.5
   libQt5Core.so.5 => /opt/qt/5.2.1/gcc_64/lib/libQt5Core.so.5

   From 2.0.5 these dependencies are added (libavahi-compat-libdnssd):
   libdns_sd.so.1 => /usr/lib/x86_64-linux-gnu/libdns_sd.so.1
   libavahi-common.so.3 => /usr/lib/x86_64-linux-gnu/libavahi-common.so.3
   libavahi-client.so.3 => /usr/lib/x86_64-linux-gnu/libavahi-client.so.3 
   
   From BeeBEEP 3.0.2 you need Qt4 libraries and Phonon:
   libphonon.so.4 => /usr/lib/i386-linux-gnu/libphonon.so.4
   libQtGui.so.4 => /usr/lib/i386-linux-gnu/libQtGui.so.4 
   libQtNetwork.so.4 => /usr/lib/i386-linux-gnu/libQtNetwork.so.4
   libQtCore.so.4 => /usr/lib/i386-linux-gnu/libQtCore.so.4 
   libQtXml.so.4 => /usr/lib/i386-linux-gnu/libQtXml.so.4 
   
      
   [ RUNTIME LIBRARIES ]

   ** Ubuntu and Debian like **
   
   For BeeBEEP 2.0
   # sudo apt-get install libqt5core5a libqt5gui5 libqt5widgets5 libqt5network5 libqt5printsupport5 libqt5multimedia5 libqt5multimedia5-plugins libxcb-screensaver0 libavahi-compat-libdnssd1
   
   For BeeBEEP 3.0
   # sudo apt-get install libqt4-dev libqt4-core libqt4-gui libqt4-xml libxcb-screensaver0 libavahi-compat-libdnssd1 libphonon4
   
   
   ** Arch LINUX **
   
   For BeeBEEP 2.0
   # sudo pacman -Sy qt5-base qt5-multimedia libxcb-screensaver0-dbg avahi
  
   For BeeBEEP 3.0  
   # sudo pacman -Sy qt4 phonon-qt4 libxcb-screensaver0-dbg avahi
   
   
   ** Fedora 22 (thanks to Marcus Frohn) ** 
   
   For BeeBEEP 2.0
   # dnf install libxcb.so.1 libxcb-screensaver.so.0 libQt5PrintSupport.so.5 libQt5Multimedia.so.5 libQt5Widgets.so.5 libQt5Network.so.5 libQt5Gui.so.5 libQt5Core.so.5 libdns_sd.so.1 -y
   
   For BeeBEEP 3.0
   # dnf install libQtGui.so.4 ibQtNetwork.so.4 libQtCore.so.4 libQtXml.so.4 libphonon.so.4 libxcb.so.1 libxcb-screensaver.so.0 libdns_sd.so.1 -y

   
   ** RPM packages **
   http://rpmfind.net/linux/rpm2html/search.php?query=libxcb-screensaver.so.0 
   http://rpmfind.net/linux/rpm2html/search.php?query=qt5
   http://rpmfind.net/linux/rpm2html/search.php?query=qt4
   http://rpmfind.net/linux/rpm2html/search.php?query=libdns_sd.so.1

   
   [ DEVELOPER LIBRARIES ]
   
   For BeeBEEP 2.0
   # sudo apt-get install build-essential libxcb-screensaver0-dev qt5-default qtmultimedia5-dev libqt5multimedia5-plugins libavahi-compat-libdnssd-dev
   * if qt5-default is not available remove it and add these packages: qtbase5-dev qtchooser qt5-qmake qtbase-dev-tools
   
   For BeeBEEP 3.0
   # sudo apt-get install build-essential qt4-default libqt4-dev libqt4-core libqt4-gui libqt4-xml libxcb-screensaver0-dev libxcb-screensaver0 libavahi-compat-libdnssd-dev libphonon-dev
   