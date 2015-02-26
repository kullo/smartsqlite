#!/usr/bin/env python3
#
# This script is used on Linux, OS X and Windows.
# Python 3 required.
# Returns 0 if project files are unchanged and 1 else.
#
# Script version: 3
import os
import glob
import hashlib
import sys

matches = []

tmp_file = "projectfiles.md5.tmp"
exlude_dirs = set(['.git', 'docs'])

def get_subdirs(path):
    return set([name for name in os.listdir(path)
        if os.path.isdir(os.path.join(path, name))])

def find_in(path):
    # print(path)
    out = []
    out += glob.glob(path + "/*.pro")
    out += glob.glob(path + "/CMakeLists.txt")
    out += glob.glob(path + "/Info.plist")
    subs = get_subdirs(path) - exlude_dirs
    for s in subs:
        out += find_in(os.path.join(path, s))
    out.sort()
    return out
	
pros = find_in(".")
# print(pros)

hasher = hashlib.md5()
for pro in pros:
    with open(pro) as f: s = f.read()
    hasher.update(s.encode('utf8'))

current = hasher.hexdigest()
if os.path.isfile(tmp_file):
    with open(tmp_file) as f: old = f.read()
else:
    old = ""

if current.strip() == old.strip():
    sys.exit(0)
else:
    with open(tmp_file, "w") as f: print(current, file=f)
    sys.exit(1)
