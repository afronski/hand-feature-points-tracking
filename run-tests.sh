#!/bin/bash

for file in assets/*.avi; do
  echo "Processing $file..."
  ./bin/tracking $file "Random Forest Tracker"
done