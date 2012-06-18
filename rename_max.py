#!/usr/bin/python2

import sys, os, re
from shutil import copy

if __name__ == "__main__":
    filePath = sys.argv[1]
    newFilePath = re.sub("\d+\.","",filePath)
    if not os.path.exists(newFilePath):
        copy(filePath, newFilePath)