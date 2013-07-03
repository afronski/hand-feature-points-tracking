killall -s9 evince
killall -s9 pdflatex
killall -s9 make
make mrproper
make cleanf
make
evince MainDocument.pdf &
