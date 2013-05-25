#!/bin/sh
rm -f videos-converted/*.webm

# Backup tracking results.
BACKUP=backup/`date +%m%d%y-%H%M%S`
mkdir -p $BACKUP
mv *_tracking_result_for_*.avi $BACKUP

# Convert videos to WEBM format.
for file in *.avi; do
  ffmpeg -i $file -q:v 0 -vcodec libvpx videos-converted/${file%%.*}.webm
done