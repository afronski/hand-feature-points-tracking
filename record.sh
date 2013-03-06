#!/bin/sh

if [ $# -eq 1 ]; then
  ffmpeg -f video4linux2 -r 12 -s 640x480 -i /dev/video0 $1.avi
elif [ $# -eq 2 ]; then
  ffmpeg -f video4linux2 -r $2 -s 640x480 -i /dev/video0 $1.avi
else
  echo "Missing at least file name for output AVI (pass without extensions) :("
  echo "As a second parameter you can pass also a frame rate in Hz ;)"
fi