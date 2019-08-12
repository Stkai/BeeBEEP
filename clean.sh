#!/bin/sh
echo "Making dist-clean..."
make distclean
echo "Cleaning BeeBEEP object files..."
rm -rf build
echo "Removing Makefile..."
rm -f Makefile
echo "Done."
