#!/bin/sh
rm -f videos-converted/*.webm
for file in *.avi; do ffmpeg -i $file videos-converted/${file%%.*}.webm; done