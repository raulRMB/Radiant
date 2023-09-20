import os
import json
import requests
from fastcdc import fastcdc
from hashlib import sha256
import threading
import time
import concurrent.futures

def loadChangeLog():
    try:
        changeLogFile = open(appDataPath + '/changelog.json')
        changeLog = json.load(changeLogFile)
        return changeLog
    except:
        changeLogFile = {}
        return changeLogFile


serverPath = "test/fakePatchServer"
installDirectory = "test/fakeClient/installDir"
appDataPath = "test/fakeClient/appData"
tempSuffix = "-temp-85ce84a3-8b4a-4c47-b1c7-17c2a8c6a69b"
blockSize = 65536

changeLog = loadChangeLog()

def main():
    ensureDirsExist()
    newBuild, oldBuild = loadPatchData()
    filesToPatch = getFilesToPatch(newBuild, oldBuild)
    localBlocks = generateBlocksForFilesToPatch(filesToPatch)
    whichBlocksAreMissing(filesToPatch, newBuild, localBlocks)
    whichBundlesShouldWeDownload(newBuild, localBlocks)
    patchFiles(newBuild, localBlocks, filesToPatch)
    cleanupDir(newBuild)
    open(appDataPath + '/changelog.json', 'w').write(str(json.dumps(changeLog)))
    open(appDataPath + '/patchData.json', 'w').write(str(json.dumps(newBuild)))

def ensureDirsExist():
    os.makedirs(os.path.abspath(installDirectory), exist_ok=True)
    os.makedirs(os.path.abspath(appDataPath), exist_ok=True)

def whichBlocksAreMissing(filesToPatch, newBuild, localBlocks):
    missingBlocks = []
    for file in filesToPatch:
        for block in newBuild[file]["blocks"]:
            if block not in localBlocks:
                missingBlocks.append(block)
    return missingBlocks

def whichBundlesShouldWeDownload(newBuild, localBlocks):
    bundlePercent = {}
    for hash in localBlocks:
        print(hash)
    for bundle in newBuild["bundles"]:
        localCount = 0
        size = 0
        for block in newBuild["bundles"][bundle]:
            #print(block)
            if block in localBlocks:
                localCount += 1
            size += 1
        bundlePercent[bundle] = localCount / size
        # print(localCount)
        # if localCount > 0:
        #     print(localCount)
    return bundlePercent
        

def loadPatchData():
    clientMap : dict = None
    if os.path.exists(appDataPath + '/patchData.json'):
        clientMap = open(appDataPath + '/patchData.json')
    serverMap = open(serverPath + '/patchData.json')
    newBuild = json.load(serverMap)
    oldBuild : dict = None
    if clientMap:
        oldBuild = json.load(clientMap)
    return newBuild, oldBuild

def mockRequestBlock(hash):
    with open(serverPath + "/blocks/" + hash, 'rb') as file:
        return file.read(-1)

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
            print(file + " needs patching")
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
    print(len(localBlocks))
    if block in localBlocks:
        return localBlocks[block]
    else:
        return mockRequestBlock(block)
        res = requests.get('http://localhost:3000/patch/block/' + block)
        return res.content

def invalidFileMetadata(file):
    filePath = installDirectory + '/' + file
    size = str(os.path.getsize(filePath))
    lastMod = str(os.path.getmtime(filePath))
    try:
        return changeLog[file]["size"] != size or changeLog[file]["lastMod"] != lastMod
    except:
        return True

def setFileMetadata(file):
    filePath = installDirectory + '/' + file
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
        del changeLog[relName]
        os.remove(fileName)

start_time = time.time()
main()
print("--- %s seconds ---" % (time.time() - start_time))