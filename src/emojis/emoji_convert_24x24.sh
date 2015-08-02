#!/bin/sh

for file in people/*.png; do convert $file -resize 24x24 $file; done
for file in nature/*.png; do convert $file -resize 24x24 $file; done
for file in objects/*.png; do convert $file -resize 24x24 $file; done
for file in places/*.png; do convert $file -resize 24x24 $file; done
for file in symbols/*.png; do convert $file -resize 24x24 $file; done


