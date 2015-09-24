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

DEBIAN
sudo apt-get install libxcb-screensaver0
sudo apt-get install qt5-default
sudo apt-get install libqt5multimedia5
sudo apt-get install libqt5multimedia5-plugins


FEDORA 22 (thanks to Marcus Frohn)

# dnf install libxcb.so.1 libxcb-screensaver.so.0 libQt5PrintSupport.so.5 libQt5Multimedia.so.5 libQt5Widgets.so.5 libQt5Network.so.5 libQt5Gui.so.5 libQt5Core.so.5 -y
# chmod +x /beebeep-folder/beebeep


RPM:
http://rpmfind.net/linux/rpm2html/search.php?query=libxcb-screensaver.so.0 
http://rpmfind.net/linux/rpm2html/search.php?query=qt5

