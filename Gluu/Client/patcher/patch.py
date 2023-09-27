import os
import json
import platform
from os.path import expanduser

import requests
import asyncio
import concurrent.futures
import aiohttp
import aiofiles
import aiofiles.os

from fastcdc import fastcdc
from hashlib import sha256
import time

serverUrl = "https://rtb.nyc3.cdn.digitaloceanspaces.com"

async def fetch(s, url, id, json=False):
    async with s.get(url) as r:
        if r.status != 200:
            r.raise_for_status()
        if json:
            data = await r.json()
        else:
            data = await r.read()
        await asyncio.sleep(0)
        return data, id

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

async def main():
    
    start_time = time.time()
    await ensureDirsExist()
    newBuild, oldBuild = loadPatchData()
    filesToPatch = getFilesToPatch(newBuild, oldBuild)
    localBlocks = generateBlocks(filesToPatch)
    missingBlocks = getMissingBlocks(filesToPatch, newBuild, localBlocks)
    bundleInfo = analyzeBundles(newBuild, missingBlocks)
    missingBlocks, bundles = bundlesToDownload(bundleInfo, missingBlocks)
    print("--- %s seconds ---" % (time.time() - start_time))

    start_time = time.time()
    connector = aiohttp.TCPConnector(limit=500)
    async with aiohttp.ClientSession(connector=connector) as session:
        await downloadBundles(bundles, newBuild, localBlocks, session)
        await downloadBlocks(missingBlocks)
    print("--- %s seconds ---" % (time.time() - start_time))

    start_time = time.time()
    await patchFiles(newBuild, localBlocks, filesToPatch)
    cleanupDir(newBuild)
    writeJsonDataToFile(changeLog, appDataPath + '/changelog.json')
    writeJsonDataToFile(newBuild, appDataPath + '/patchData.json')
    print("--- %s seconds ---" % (time.time() - start_time))

def writeJsonDataToFile(data, file):
    try:
        x = open(file, 'w')
        x.write(str(json.dumps(data)))
        x.close()
    except Exception as e:
        print(e)

def bundlesToDownload(bundleInfo, missingBlocks):
    toDownload = {}
    for bundle in bundleInfo:
        if bundleInfo[bundle]["percentNeeded"] >= 0.5:
            toDownload[bundle] = bundleInfo[bundle]["blocksNeeded"]
            for block in bundleInfo[bundle]["blocksNeeded"]:
                missingBlocks.remove(block)
    return missingBlocks, toDownload

async def downloadBlocks(missingBlocks):
    tasks = []
    for block in missingBlocks:
        task = asyncio.create_task(fetch(s, serverUrl + '/patchData/blocks/' + block, block))
        tasks.append(task)
    for task in asyncio.as_completed(tasks):
        resp, id = await task
        localBlocks[id] = resp

async def downloadBundles(bundles, newBuild, localBlocks, s):
    tasks = []
    for bundle in bundles:
        task = asyncio.create_task(fetch(s, serverUrl + '/patchData/bundles/' + bundle, bundle))
        tasks.append(task)
    for task in asyncio.as_completed(tasks):
        resp, id = await task
        for i in newBuild["bundles"][id]:
            block = newBuild["bundles"][id][i]
            if block["hash"] in localBlocks:
                continue
            length = block["length"]
            offset = block["blockOffset"]
            splice = resp[offset:(offset + length)]
            localBlocks[block["hash"]] = splice

async def ensureDirsExist():
    await aiofiles.os.makedirs(os.path.abspath(installDirectory).replace(os.sep, '/'), exist_ok=True)
    await aiofiles.os.makedirs(os.path.abspath(appDataPath).replace(os.sep, '/'), exist_ok=True)

def getMissingBlocks(filesToPatch, newBuild, localBlocks):
    missingBlocks = set()
    for file in filesToPatch:
        for block in newBuild[file]["blocks"]:
            if block not in localBlocks:
                missingBlocks.add(block)
    return missingBlocks

def analyzeBundles(newBuild, missingBlocks):
    bundleInfo = {}
    for bundle in newBuild["bundles"]:
        bundleSize = 0
        blocksInThisBundleWeNeed = 0
        neededBlocks = []
        for block in newBuild["bundles"][bundle]:
            if newBuild["bundles"][bundle][block]["hash"] in missingBlocks:
                neededBlocks.append(newBuild["bundles"][bundle][block]["hash"])
                blocksInThisBundleWeNeed += 1
            bundleSize += 1
        if bundleSize != 0:
            bundleInfo[bundle] = {"percentNeeded": blocksInThisBundleWeNeed / bundleSize, "blocksNeeded": neededBlocks}
    return bundleInfo
        
def loadPatchData():
    clientMap : dict = None
    if os.path.exists(appDataPath + '/patchData.json'):
        clientMap = open(appDataPath + '/patchData.json')
    newBuild = requests.get(serverUrl + '/patchData/patchData.json').json()
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

def generateBlocks(filesToPatch):
    blocks = dict()
    args = []
    for file in filesToPatch:
        filePath = installDirectory + '/' + file
        args.append((filePath, blocks))
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as executor:
        for _ in executor.map(runFastOnFile, args):
            pass   
    return blocks

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

async def patchFiles(newBuild, localBlocks, filesToPatch):
    args = []
    for file in filesToPatch:
        args.append((file, newBuild, localBlocks))
    for call in args:
        await applyPatchToFile(call)

async def applyPatchToFile(args):
    file = args[0]
    newBuild = args[1]
    localBlocks = args[2]
    tempFile = createTempFile(file)
    for block in newBuild[file]["blocks"]:
        tempFile.write(localBlocks[block])
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

if __name__ == '__main__':
    # start_time = time.time()
    asyncio.run(main())
    # print("--- %s seconds ---" % (time.time() - start_time))
    print("DONE", end='')