from fastcdc import fastcdc
from hashlib import sha256
import json
import time
import os
import concurrent.futures
import zstandard as zstd
import multiprocessing

directory = "test/fakeBuild"
outputPath = "../Gluu/Platform/ServerBackend/patchData"
blockPath = outputPath + "/blocks"
bundlePath = outputPath + "/bundles"
blockSize = 65536
blockSet = {}


def main():
    ensureDirsExist()
    data = processDir(directory)
    buildBundles(data)
    writePatchData(data)
    cleanupBlocks(data)

def buildBundles(data):
    blocksProcessed = 0
    bundleList = {}
    bundleData = b''
    length = 0
    for block in blockSet:
        result = blockSet[block]
        bundleList[blocksProcessed] = {"hash": result.hash, "length": len(result.data), "blockOffset": length}   
        length += len(result.data)
        bundleData = bundleData + result.data
        if(blocksProcessed == 59):
            if(len(bundleData) != length):
                print("!")
            createBundle(bundleList, bundleData, data)
            bundleList = {}
            bundleData = b''
            blocksProcessed = 0
            length = 0
        else:
            blocksProcessed += 1

def getBundleId(bundleList):
    return sha256(str(bundleList).encode('utf-8')).hexdigest()

def writeBundle(bundleList, bundleData, id):
    outputFilename = os.path.join(bundlePath + '/' + id)
    if not os.path.exists(outputFilename):
            with open(outputFilename, 'wb') as outputFile:
                outputFile.write(bundleData)

def createBundle(bundleList, bundleData, data):
    id = getBundleId(bundleList)
    writeBundle(bundleList, bundleData, id)
    data["bundles"][id] = bundleList

def ensureDirsExist():
    os.makedirs(os.path.abspath(directory), exist_ok=True)
    os.makedirs(os.path.abspath(blockPath), exist_ok=True)
    os.makedirs(os.path.abspath(bundlePath), exist_ok=True)

def cleanupBlocks(data):
    args = []
    if os.path.exists(blockPath):
        for root, dirs, files in os.walk(blockPath):
            for file in files:
                if(file not in blockSet):
                    args.append(os.path.join(root, file))
    with concurrent.futures.ThreadPoolExecutor(max_workers=20) as executor:
        for _ in executor.map(removeBlock, args):
            pass

def removeBlock(path):
    os.remove(path.replace(os.sep, '/'))

def getRelPath(filePath, directory):
    relPath = os.path.relpath(filePath, directory)
    return relPath.replace(os.sep, '/')

def processBlock(result, fileData, fileHash, data):
    #c = zstd.ZstdCompressor(level=19)
    #compressed = c.compress(result.data)
    outputFilename = os.path.join(blockPath, result.hash)
    if not os.path.exists(outputFilename):
        with open(outputFilename, 'wb') as outputFile:
            outputFile.write(result.data)
    fileHash.append(result.hash)
    blockSet[result.hash] = result
    fileData["blocks"].append(result.hash)

def processFile(args):
    path = args[0]
    relPath = args[1]
    data = args[2]
    fileData = {"hash" : "", "blocks": []}
    fast = fastcdc(path, blockSize * .5, blockSize, blockSize * 2, True, sha256)
    results = list(fast)
    fileHash = list()
    for result in results:
        processBlock(result, fileData, fileHash, data)
    fileData["hash"] = sha256(''.join(fileHash).encode('utf-8')).hexdigest()
    data[relPath] = fileData

def processDir(installDirectory):
    data = {"bundles": {}}
    args = []
    for root, subdirs, files in os.walk(os.path.abspath(directory)):
        for filename in files:
            filePath = os.path.join(root, filename)
            relPath = getRelPath(filePath, directory)
            args.append((filePath, relPath, data))
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as executor:
        for _ in executor.map(processFile, args):
            pass
    return data

def writePatchData(data):
    maps = open(outputPath + "/patchData.json", "w")
    maps.write(str(json.dumps(data, indent=4)))
    maps.close()

start_time = time.time()
main()
print("--- %s seconds ---" % (time.time() - start_time))
