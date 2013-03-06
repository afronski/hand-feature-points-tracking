#!/bin/bash

# Build all applications.
scons

# Download and configure dependencies.
npm install
cp config/vidStreamer.json node_modules/vid-streamer/config

# Invoke custom scripts.
pushd assets
./convert.sh
popd

# Run server and client.
node server/server.js &
chromium http://localhost:9292 &