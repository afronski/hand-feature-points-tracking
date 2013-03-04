#!/bin/bash

# Build all applications.
scons

# Run server and client.
node server/server.js &
chromium http://localhost:9292 &