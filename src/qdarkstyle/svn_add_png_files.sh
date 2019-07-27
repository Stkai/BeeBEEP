#!/bin/sh
find rc/ -iname '*.png' -exec svn add "{}@" \;
