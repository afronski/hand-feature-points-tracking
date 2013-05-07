#!/bin/sh

if [ $# -eq 1 ]; then
  INPUT=$1
  OUTPUT=videos-converted/${INPUT%%.*}.webm

  pushd assets > /dev/null
    rm -f $OUTPUT
    ffmpeg -i $INPUT -q:v 0 -vcodec libvpx $OUTPUT
  popd > /dev/null
else
  echo "Missing file name for conversion :("
fi