#!/bin/sh
NCORES=$(nproc)
MAXCORES=$(( NCORES - 3 ))
NJS=$(( MAXCORES > 1 ? MAXCORES : 1 ))
echo "Cleaning previous compiled files..."
rm -f test/beebeep
rm -f test/*.so*
echo "Creating BeeBEEP Makefile..."
qmake -o Makefile beebeep-desktop.pro
echo "Compiling BeeBEEP with $NJS threads ($NCORES cores)..."
make -j$NJS all
echo "Done."
