echo BeeBEEP fro OS/2 qt 5
set MAKESHELL="cmd.exe"
qmake-qt5 -o Makefile beebeep-desktop.pro
make all
echo "Done."
