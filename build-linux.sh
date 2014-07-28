#!/bin/bash
set -e  # exit immediately on error
set -v  # verbose, echo all commands

SOURCE_DIR=$(pwd)
BUILD_DIR="../build-sqlitewrapper"
CMAKE=cmake
JOBS=`nproc`
echo $JOBS
MAKE="make --jobs=$JOBS"


if ! python3 projectfiles_unchanged.py ; then
  echo ".pro files changed. Removing build directory '$BUILD_DIR'"
  rm -r "$BUILD_DIR" || true
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

$CMAKE "$SOURCE_DIR"
$MAKE

cd "$SOURCE_DIR"
