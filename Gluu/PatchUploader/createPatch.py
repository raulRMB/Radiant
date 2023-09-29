from fastcdc import fastcdc
from hashlib import sha256
import json
import time
import os
import concurrent.futures
import zstandard as zstd
import multiprocessing
import random
from boto3 import session
from botocore.client import Config
from dotenv import load_dotenv
from pydo import Client


load_dotenv()

ACCESS_KEY = os.getenv('ACCESS_KEY')
SECRET_KEY = os.getenv('SECRET_KEY')
DO_TOKEN = os.getenv('DO_TOKEN')

directory = "test/fakeBuild"
outputPath = "../Backend/data/patchData"
blockPath = outputPath + "/blocks"
bundlePath = outputPath + "/bundles"
blockSize = 65536
blockSet = {}
bundleSet = {}

uncompressedBlockData = []

clientDO = Client(DO_TOKEN)


def main():
    ensureDirsExist()
    data = processDir(directory)
    train_time = time.time()
    dict_data = zstd.train_dictionary(blockSize, random.sample(uncompressedBlockData, 2000))
    if os.path.exists(outputPath + '/dictionary'):
        os.remove(outputPath + '/dictionary')
    with open(outputPath + '/dictionary', 'wb') as outputFile:
        outputFile.write(dict_data.as_bytes())
    print("--- train time %s seconds ---" % (time.time() - train_time))
    buildBlocks(dict_data)
    buildBundles(data)
    writePatchData(data)
    validateBlocks(data)
    cleanup(data, blockPath, blockSet)
    cleanup(data, bundlePath, data["bundles"])

s3cmd = 'python C:/s3cmd/s3cmd'
testData = 'C:/DigitalOcean/test.txt'
bucketFile = 's3://rtb/test1.txt'

def updatePatchVersionFile():
    sesh = session.Session()
    client = sesh.client(
        's3',
        region_name='nyc3',
        endpoint_url='https://nyc3.digitaloceanspaces.com',
        aws_access_key_id=ACCESS_KEY,
        aws_secret_access_key=SECRET_KEY)

    fileName = 'test/test1.txt'
    client.upload_file(testData, 'rtb', fileName, {'ACL': 'public-read', 'ContentType': 'binary/octet-stream'})

    purge_req = {"files": [fileName]}
    clientDO.cdn.purge_cache("12148a1d-4fdf-4925-a0c7-fad213c75b7b", purge_req)

def buildBlocks(dict_data):
    b_time = time.time()
    c = zstd.ZstdCompressor(level=5, dict_data=dict_data, threads=-1)
    for block in blockSet:
        compressed = c.compress(blockSet[block].data)
        blockSet[block] = compressed
        outputFilename = os.path.join(blockPath, block)
        if not os.path.exists(outputFilename):
            with open(outputFilename, 'wb') as outputFile:
                outputFile.write(compressed)
    print("--- block time %s seconds ---" % (time.time() - b_time))
    
def buildBundles(data):
    blocksProcessed = 0
    bundleList = {}
    bundleData = []
    length = 0
    for block in blockSet:
        result = blockSet[block]
        bundleList[blocksProcessed] = {"hash": block, "length": len(result), "blockOffset": length}   
        length += len(result)
        bundleData.append(result)
        if(blocksProcessed == 59):
            createBundle(bundleList, b''.join(bundleData), data)
            bundleList = {}
            bundleData = []
            blocksProcessed = 0
            length = 0
        else:
            blocksProcessed += 1
    createBundle(bundleList, b''.join(bundleData), data)

def validateBlocks(data):
    missing = []
    blockCount = 0
    for file in data:
        if file == "bundles":
            continue
        for block in data[file]["blocks"]:
            blockCount += 1
            outputFilename = os.path.join(blockPath + '/' + block)
            if not os.path.exists(outputFilename):
                missing.append(block)
    if(len(missing) > 0):
        print(f'Missing {len(missing)} of {blockCount} blocks!')
    else:
        print('Validated that patchData matches blocks')

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

def cleanup(data, path, _set):
    args = []
    if os.path.exists(path):
        for root, dirs, files in os.walk(path):
            for file in files:
                if(file not in _set):
                    args.append(os.path.join(root, file))
    with concurrent.futures.ThreadPoolExecutor(max_workers=20) as executor:
        for _ in executor.map(removeBlock, args):
            pass

def removeBlock(path):
    os.remove(path.replace(os.sep, '/'))

def getRelPath(filePath, directory):
    relPath = os.path.relpath(filePath, directory)
    return relPath.replace(os.sep, '/')

def processFile(args):
    path = args[0]
    relPath = args[1]
    data = args[2]
    fileData = {"hash" : "", "blocks": []}
    fast = fastcdc(path, blockSize * .5, blockSize, blockSize * 2, True, sha256)
    results = list(fast)
    fileHash = list()
    for result in results:
        uncompressedBlockData.append(result.data)
        blockSet[result.hash] = result
        fileHash.append(result.hash)
        fileData["blocks"].append(result.hash)
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
    maps.write(str(json.dumps(data)))
    maps.close()

start_time = time.time()
main()
print("--- %s seconds ---" % (time.time() - start_time))
