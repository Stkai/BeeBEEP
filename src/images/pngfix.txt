#!/bin/sh
if [ $# -gt 0 ]; then
  echo "Crushing $1"
  pngcrush -rem allb -brute "$1" temp.png
  mv -f temp.png $1
  echo "Done"
  exit 0
else
  echo "Usage: $0 file.png"
  exit 1
fi

