#!/bin/sh

new_size=24

for file in people/*.png; do convert $file -resize x$new_size $file; done
for file in nature/*.png; do convert $file -resize x$new_size $file; done
for file in objects/*.png; do convert $file -resize x$new_size $file; done
for file in places/*.png; do convert $file -resize x$new_size $file; done
for file in symbols/*.png; do convert $file -resize x$new_size $file; done


