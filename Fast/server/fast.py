from fastcdc import fastcdc
from hashlib import sha256
from rwbin import rwBinRange
import json
import os

if os.path.exists('chonks'):
    for root, dirs, files in os.walk('chonks'):
        for file in files:
            os.remove(os.path.join(root, file))

installDirectory = "C:\\Radiant\\Fast\\server\\build"
dic = {"fileinfo" : {}}
blocks = dict()

def process_file(path, relPath) -> dict:
    outDict = {"hash" : "", "blocks" : {}}
    fast = fastcdc(path, 65536 * .5, 65536, 65536 * 2, True, sha256)
    results = list(fast)
    idx = 0
    hashes = ''
    for result in results:
        rwBinRange('chonks/', result)
        hashes += result.hash
        blocks[result.hash] = [relPath, idx, result.offset, result.length]
        outDict["blocks"][idx] = {"startindex" : result.offset, "length" : result.length, "hash" : result.hash}
        idx += 1
    outDict["hash"] = sha256(hashes.encode('utf-8')).hexdigest()
    
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
