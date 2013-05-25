#!/bin/bash

# Build all applications.
scons $1

# Download and configure dependencies.
npm install
cp config/vidStreamer.json node_modules/vid-streamer/config

# Preparation.
pkill node

# Run server and client.
node server/server.js &
chromium http://localhost:9292 &