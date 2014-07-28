# version: 1
import os
import glob
import hashlib
import sys

matches = []

exlude_dirs = set(['.git', 'docs'])

def get_subdirs(path):
    return set([name for name in os.listdir(path)
        if os.path.isdir(os.path.join(path, name))])

def find_in(path):
    # print(path)
    out = []
    out += glob.glob(path + "/*.pro")
    out += glob.glob(path + "/CMakeLists.txt")
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
if os.path.isfile("projectfiles.md5.tmp"):
    with open("projectfiles.md5.tmp") as f: old = f.read()
else:
    old = ""

if current.strip() == old.strip():
    sys.exit(0)
else:
    with open("projectfiles.md5.tmp", "w") as f: print(current, file=f)
    sys.exit(1)
