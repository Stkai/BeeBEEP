echo BeeBEEP fro OS/2 qt 4
set MAKESHELL="cmd.exe"
qmake -o Makefile beebeep-desktop.pro
make all
echo "Done."
