#!/bin/sh

if [ $# -eq 1 ]; then
  INPUT=$1
  OUTPUT=videos-converted/${INPUT%%.*}.webm

  pushd assets > /dev/null
    rm -f $OUTPUT
    ffmpeg -i $INPUT -r 25 -f webm -b:v 20M $OUTPUT
  popd > /dev/null
else
  echo "Missing file name for conversion :("
fi