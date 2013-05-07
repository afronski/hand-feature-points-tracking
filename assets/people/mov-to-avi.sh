#!/bin/sh

# Remove previously converted samples.
rm ../Person_*_Sample*.avi

# First pack of samples.
pushd "PersonA"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_A_${file%%.*}.avi
done

popd