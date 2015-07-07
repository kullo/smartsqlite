#!/bin/bash

set -e  # exit immediately on error
set -v  # verbose, echo all commands

export SDKROOT=macosx10.9

SOURCE_DIR=$(pwd)
BUILD_DIR="../build-smartsqlite"
CMAKE_COMMAND=cmake
MAKE_COMMAND="make -j2"

export PATH="/usr/local/bin:$PATH"

if ! python3 projectfiles_unchanged.py ; then
  echo "Project files changed. Removing build directory '$BUILD_DIR'"
  rm -r "$BUILD_DIR" || true
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

$CMAKE_COMMAND "$SOURCE_DIR"
$MAKE_COMMAND

cd "$SOURCE_DIR"
