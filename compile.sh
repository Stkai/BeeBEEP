#!/bin/sh
echo "Creating BeeBEEP Makefile..."
qmake -o Makefile beebeep-desktop.pro
echo "Compiling BeeBEEP..."
make all
echo "Done."
