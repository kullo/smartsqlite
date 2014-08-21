#!/bin/sh
cppcheck src -i sqlite3.c -I include -USQLITE_INT64_TYPE --enable=warning,style,performance,portability,information --quiet
