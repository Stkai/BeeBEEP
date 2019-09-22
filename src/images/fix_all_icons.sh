#!/bin/sh
for png in `find . -iname "*.png"`;
  do
  echo "Crushing $png"
  pngcrush -rem allb -brute "$png" temp.png
  mv -f temp.png $png
done;

