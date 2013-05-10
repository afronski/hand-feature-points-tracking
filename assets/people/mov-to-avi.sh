#!/bin/sh

# Remove previously converted samples.
rm ../Person_*_*.avi

# People - packs of samples.
pushd "PersonA"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_A_${file%%.*}.avi
done

popd

pushd "PersonB"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_B_${file%%.*}.avi
done

popd

pushd "PersonC"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_C_${file%%.*}.avi
done

popd

pushd "PersonD"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_D_${file%%.*}.avi
done

popd

pushd "PersonE"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_E_${file%%.*}.avi
done

popd

pushd "PersonF"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_F_${file%%.*}.avi
done

popd

pushd "PersonG"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_G_${file%%.*}.avi
done

popd

pushd "PersonH"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_H_${file%%.*}.avi
done

popd

pushd "PersonI"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_I_${file%%.*}.avi
done

popd

pushd "PersonJ"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_J_${file%%.*}.avi
done

popd

# Last pack of samples (mostly testing sequences).
pushd "PersonZ"

for file in *.MOV; do
  ffmpeg -i "$file" -q:v 0 -vf "transpose=2" -vcodec msmpeg4v2 ../../Person_Z_${file%%.*}.avi
done

popd