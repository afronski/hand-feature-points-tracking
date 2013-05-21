#!/bin/sh

pushd bin/training-base-directory/ > /dev/null
  rm -rf *
  touch .gitkeep
popd > /dev/null