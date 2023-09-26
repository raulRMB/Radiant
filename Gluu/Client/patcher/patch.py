import os
import json
import requests
import platform
from os.path import expanduser

from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry
from fastcdc import fastcdc
from hashlib import sha256
import threading
import time
import concurrent.futures

serverUrl = "http://localhost:3000"

session = requests.Session()
retry = Retry(connect=3, backoff_factor=0.5)
adapter = HTTPAdapter(max_retries=retry)
session.mount('http://', adapter)
session.mount('https://', adapter)

def loadChangeLog():
    try:
        changeLogFile = open(appDataPath + '/changelog.json')
        changeLog = json.load(changeLogFile)
        return changeLog
    except:
        changeLogFile = {}
        return changeLogFile

folderName = "/RadiantGames"
platform = platform.system()

if platform == 'Windows':
    installDirectory = "C:/Program Files" + folderName + '/install'
    appDataPath = "C:/Program Files" + folderName + '/data'
    # appDataPath = os.getenv('LOCALAPPDATA').replace(os.sep, '/') + folderName
elif platform == 'Darwin':
    installDirectory = '/Applications' + folderName
    appDataPath = expanduser("~") + '/Library/Application Support' + folderName
tempSuffix = "-temp-85ce84a3-8b4a-4c47-b1c7-17c2a8c6a69b"
blockSize = 65536

changeLog = loadChangeLog()

def main():
    ensureDirsExist()
    newBuild, oldBuild = loadPatchData()
    filesToPatch = getFilesToPatch(newBuild, oldBuild)
    localBlocks = generateBlocksForFilesToPatch(filesToPatch)
    missingBlocks = whichBlocksAreMissing(filesToPatch, newBuild, localBlocks)
    bundlePercents = whichBundlesShouldWeDownload(newBuild, missingBlocks)
    bundles = bundlesToDownload(bundlePercents)
    downloadBundles(bundles, newBuild, localBlocks)
    patchFiles(newBuild, localBlocks, filesToPatch)
    cleanupDir(newBuild)
    writeJsonDataToFile(changeLog, appDataPath + '/changelog.json')
    writeJsonDataToFile(newBuild, appDataPath + '/patchData.json')

def writeJsonDataToFile(data, file):
    try:
        x = open(file, 'w')
        x.write(str(json.dumps(data)))
        x.close()
    except Exception as e:
        print(e)

def bundlesToDownload(bundlePercents):
    toDownload = []
    for bundle in bundlePercents:
        if bundlePercents[bundle] >= 0.5:
            toDownload.append(bundle)
    return toDownload

def downloadBundles(bundles, newBuild, localBlocks):
    count = 0
    total = len(bundles)
    for bundle in bundles:
        res = session.get(serverUrl + '/patch/bundle/' + bundle)
        for i in newBuild["bundles"][bundle]:
            block = newBuild["bundles"][bundle][i]
            length = block["length"]
            offset = block["blockOffset"]
            splice = res.content[offset:(offset + length)]
            localBlocks[block["hash"]] = splice
        count += 1
        print(f'{round(count/total * 100)}', end='')

def ensureDirsExist():
    os.makedirs(os.path.abspath(installDirectory).replace(os.sep, '/'), exist_ok=True)
    os.makedirs(os.path.abspath(appDataPath).replace(os.sep, '/'), exist_ok=True)

def whichBlocksAreMissing(filesToPatch, newBuild, localBlocks):
    missingBlocks = set()
    for file in filesToPatch:
        for block in newBuild[file]["blocks"]:
            if block not in localBlocks:
                missingBlocks.add(block)
    return missingBlocks

def whichBundlesShouldWeDownload(newBuild, missingBlocks):
    bundlePercent = {}
    for bundle in newBuild["bundles"]:
        bundleSize = 0
        blocksInThisBundleWeNeed = 0
        for block in newBuild["bundles"][bundle]:
            if newBuild["bundles"][bundle][block]["hash"] in missingBlocks:
                blocksInThisBundleWeNeed += 1
            bundleSize += 1
        if bundleSize != 0:
            bundlePercent[bundle] = blocksInThisBundleWeNeed / bundleSize
    return bundlePercent
        
def loadPatchData():
    clientMap : dict = None
    if os.path.exists(appDataPath + '/patchData.json'):
        clientMap = open(appDataPath + '/patchData.json')
    newBuild = session.get(serverUrl + '/patch/info/latest').json()
    oldBuild : dict = None
    if clientMap:
        oldBuild = json.load(clientMap)
    return newBuild, oldBuild

def runFastOnFile(args) -> dict:
    path = args[0] 
    blocks = args[1]
    if os.path.exists(path):
        fast = fastcdc(path, blockSize * .5, blockSize, blockSize * 2, True, sha256)
        results = list(fast)
        for result in results:
            blocks[result.hash] = result.data

def getFilesToPatch(newBuild, oldBuild):
    filesToPatch = list()
    for file in newBuild:
        if file == "bundles":
            continue
        if oldBuild == None or file not in oldBuild or newBuild[file]["hash"] != oldBuild[file]["hash"] or invalidFileMetadata(file):
            filesToPatch.append(file)
    return filesToPatch

def generateBlocksForFilesToPatch(filesToPatch):
    blocks = dict()
    args = []
    for file in filesToPatch:
        filePath = installDirectory + '/' + file
        args.append((filePath, blocks))
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as executor:
        for _ in executor.map(runFastOnFile, args):
            pass   
    return blocks

def resolveBlock(block, localBlocks):
    if block in localBlocks:
        return localBlocks[block]
    else:
        res = session.get('http://localhost:3000/patch/block/' + block)
        if res.status_code == 404:
            raise Exception(f'Block not found! {block}') 
        localBlocks[block] = res.content
        return res.content

def invalidFileMetadata(file):
    filePath = installDirectory + '/' + file
    if not os.path.exists(filePath):
        return True
    size = str(os.path.getsize(filePath))
    lastMod = str(os.path.getmtime(filePath))
    try:
        return changeLog[file]["size"] != size or changeLog[file]["lastMod"] != lastMod
    except:
        return True

def setFileMetadata(file):
    filePath = installDirectory + '/' + file
    if ".DS_Store" in filePath:
        return
    size = str(os.path.getsize(filePath))
    lastMod = str(os.path.getmtime(filePath))
    changeLog[file] = {"size": size, "lastMod": lastMod}

def createTempFile(file):
    filePath = installDirectory + '/' + file + tempSuffix
    os.makedirs(os.path.dirname(filePath), exist_ok=True)
    if os.path.exists(filePath):
        os.remove(filePath)
    return open(filePath, 'wb')

def patchFiles(newBuild, localBlocks, filesToPatch):
    args = []
    for file in filesToPatch:
        args.append((file, newBuild, localBlocks))
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as executor:
        for _ in executor.map(applyPatchToFile, args):
            pass

def applyPatchToFile(args):
    file = args[0]
    newBuild = args[1]
    localBlocks = args[2]
    tempFile = createTempFile(file)
    for block in newBuild[file]["blocks"]:
        binData = resolveBlock(block, localBlocks)
        tempFile.write(binData)
    tempFile.close()

def cleanupDir(newBuild):
    args = []
    removeFileArgs = []
    for root, subdirs, files in os.walk(os.path.abspath(installDirectory), topdown=False):
        for file in files:
            args.append((root, file, newBuild))
        removeFileArgs.append(root)
    with concurrent.futures.ThreadPoolExecutor(max_workers=20) as executor:
        for _ in executor.map(cleanupFile, args):
            pass
    for dir in removeFileArgs:
        deleteEmptyFolders(dir)
            
def deleteEmptyFolders(root):
    if len(os.listdir(root)) == 0:
        os.rmdir(root)

def cleanupFile(args):
    root = args[0]
    file = args[1]
    newBuild = args[2]
    fileName = os.path.join(root, file)
    relName = os.path.relpath(fileName, installDirectory)
    relName = relName.replace(os.sep, '/')
    if file.endswith(tempSuffix):
        name = os.path.join(root, file.replace(tempSuffix, ""))
        if os.path.exists(name):
            os.remove(name)
        os.rename(fileName, name)
        setFileMetadata(relName.replace(tempSuffix, ""))
    elif relName not in newBuild:
        if relName == ".DS_Store":
            return
        del changeLog[relName]
        os.remove(fileName)

start_time = time.time()
main()
# print("--- %s seconds ---" % (time.time() - start_time))
print("DONE", end='')