#!/bin/sh
echo "Cleaning previous compiled files..."
rm -f test/beebeep
rm -f test/*.so*
echo "Creating BeeBEEP Makefile..."
qmake -o Makefile beebeep-desktop.pro
echo "Compiling BeeBEEP..."
make all
echo "Done."
