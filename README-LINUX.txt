/****************************************************************
 *                 LINUX INSTALL GUIDE                          *
 *  Latest version on: https://beebeep.net/help                 *
 *                                                              *
 ****************************************************************/


1) Download the right version for your system (32 or 64 bits):
   https://www.beebeep.net/download

2) Uncompress the package in your favorite folder

3) From a terminal add executable permission to the application
   # chmod +x /beebeep-folder/beebeep

4) Check BeeBEEP dependencies and install packages if needed

   ** Ubuntu and Debian like **

   For BeeBEEP 2
   # sudo apt-get install libqt5core5a libqt5gui5 libqt5widgets5 libqt5network5 libqt5printsupport5 libqt5multimedia5 libqt5multimedia5-plugins libxcb-screensaver0 libavahi-compat-libdnssd1

   For BeeBEEP 3,4,5
   # sudo apt-get install libqt4-dev libqt4-core libqt4-gui libqt4-xml libxcb-screensaver0 libavahi-compat-libdnssd1 libphonon4 libhunspell-dev phonon-backend-gstreamer

   (in ubuntu 16.04)
  # sudo apt-get install libqt4-dev libqt4-xml libxcb-screensaver0 libavahi-compat-libdnssd1 libphonon4 libhunspell-dev phonon-backend-gstreamer

   ** Arch LINUX **

   For BeeBEEP 2
   # sudo pacman -Sy qt5-base qt5-multimedia libxcb-screensaver0-dbg avahi

   For BeeBEEP 3,4,5
   # sudo pacman -Sy qt4 phonon-qt4 libxcb-screensaver0-dbg avahi


   ** Fedora 22 (thanks to Marcus Frohn) **

   For BeeBEEP 2
   # dnf install libxcb.so.1 libxcb-screensaver.so.0 libQt5PrintSupport.so.5 libQt5Multimedia.so.5 libQt5Widgets.so.5 libQt5Network.so.5 libQt5Gui.so.5 libQt5Core.so.5 libdns_sd.so.1 -y

   For BeeBEEP 3,4,5
   # dnf install libQtGui.so.4 ibQtNetwork.so.4 libQtCore.so.4 libQtXml.so.4 libphonon.so.4 libxcb.so.1 libxcb-screensaver.so.0 libdns_sd.so.1 hunspell -y


   ** RPM packages **
   http://rpmfind.net/linux/rpm2html/search.php?query=libxcb-screensaver.so.0
   http://rpmfind.net/linux/rpm2html/search.php?query=qt5
   http://rpmfind.net/linux/rpm2html/search.php?query=qt4
   http://rpmfind.net/linux/rpm2html/search.php?query=libdns_sd.so.1


   [ DEVELOPER LIBRARIES ]

   For BeeBEEP 2
   # sudo apt-get install build-essential libgl1-mesa-dev libqt5x11extras5-dev libxcb-screensaver0-dev qt5-default qtmultimedia5-dev libqt5multimedia5-plugins libavahi-compat-libdnssd-dev
   * if qt5-default is not available remove it and add these packages: qtbase5-dev qtchooser qt5-qmake qtbase-dev-tools

   For BeeBEEP 3,4,5
   # sudo apt-get install build-essential qt4-default libqt4-dev libqt4-core libqt4-gui libqt4-xml libxcb-screensaver0-dev libxcb-screensaver0 libavahi-compat-libdnssd-dev libphonon-dev libhunspell-dev

