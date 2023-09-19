from fastcdc import fastcdc
from hashlib import sha256
from rwbin import rwBinRange
import json
import os

installDirectory = "C:\\Radiant\\Fast\\server\\build"
dic = {"fileinfo" : {}}
blocks = dict()

def process_file(path, relPath) -> dict:
    outDict = {}
    fast = fastcdc(path, 16384, 32768, 65536, True, sha256)
    results = list(fast)
    idx = 0
    for result in results:
        rwBinRange('outputs/', result)
        blocks[result.hash] = [relPath, idx, result.offset, result.length]
        outDict[idx] = {"startindex" : result.offset, "length" : result.length, "hash" : result.hash}
        idx += 1
    
    return outDict
  
for root, subdirs, files in os.walk(os.path.abspath(installDirectory)):
    for filename in files:
        file_path = os.path.join(root, filename)
        relPath = os.path.relpath(file_path, installDirectory)
        relPath = relPath.replace(os.sep, '/')
        dic["fileinfo"][relPath] = process_file(file_path, relPath)

maps = open("maps.json", "w")
dic["blocks"] = blocks
maps.write(str(json.dumps(dic, indent=4)))
maps.close()
