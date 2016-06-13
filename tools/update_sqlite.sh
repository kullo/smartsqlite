#!/bin/bash
set -o errexit -o nounset -o pipefail
which shellcheck > /dev/null && shellcheck "$0"


VERSION="$1"

FILENAME="sqlite-amalgamation-$VERSION"

(
    cd tmp
    wget --output-document="$FILENAME.zip" \
      "https://www.sqlite.org/2016/sqlite-amalgamation-$VERSION.zip"
    sha1sum "$FILENAME.zip"
    unzip -o "$FILENAME.zip"
)

cp "./tmp/$FILENAME/"{shell,sqlite3}.c ../src/
cp "./tmp/$FILENAME/sqlite3.h" ../include/smartsqlite
