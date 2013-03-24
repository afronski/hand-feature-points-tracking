#!/bin/sh
rm -f videos-converted/*.webm

# Backup tracking results.
BACKUP=backup/`date +%m%d%y-%H%M%S`
mkdir -p $BACKUP
mv *_tracking_result.avi $BACKUP

# Convert videos to WEBM format.
for file in *.avi; do
  ffmpeg -i $file -r 25 -f webm -b:v 20M videos-converted/${file%%.*}.webm
done