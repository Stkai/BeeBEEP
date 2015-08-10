#!/bin/sh

for png in `find $1 -name "*.png"`;
do
  echo "Crushing $png"
  pngcrush -ow -rem allb -reduce $png
done;
echo "Completed."

