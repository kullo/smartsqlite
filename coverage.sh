#!/bin/bash

BUILD_DIR="../build-sqlitewrapper-coverage"
LCOV_CAPTURE_ARGS="--directory src"
QUIET="--quiet"

# stop on error
set -e

# change to directory containing script
cd "$(dirname "$0")"

# get number of CPU cores
CORES=$(nproc)

# build
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake -DCMAKE_C_FLAGS="--coverage" -DCMAKE_CXX_FLAGS="--coverage" ../sqlitewrapper

echo "Running make..."
make --jobs "${CORES}" $QUIET

# delete old data
find . -name "*.gcda" -delete
rm coverage_*.info || true

# init stats
lcov --capture --initial --output-file coverage_base.info $LCOV_CAPTURE_ARGS $QUIET

# run tests
test/sqlitewrapper_tests

# collect stats
lcov --capture --output-file coverage_test.info $LCOV_CAPTURE_ARGS $QUIET
lcov --add-tracefile coverage_base.info --add-tracefile coverage_test.info --output-file coverage_combined.info $QUIET
lcov --remove coverage_combined.info "sqlite3.c" --output-file coverage_filtered.info $QUIET
lcov --extract coverage_filtered.info "*/src/*" --output-file coverage_total.info $QUIET
genhtml $QUIET --output-directory coverage coverage_total.info

# open html
xdg-open coverage/index.html

