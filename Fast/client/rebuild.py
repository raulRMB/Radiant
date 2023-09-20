import os
import json
import rwbin
from fastcdc import fastcdc
from hashlib import sha256

filesToPatch = list()
installDirectory = "C:/Radiant/Fast/client/build"
appDataPath = "C:/Radiant/Fast/"
clientMap : dict = None
if os.path.exists(appDataPath + 'client/maps.json'):
    clientMap = open(appDataPath + 'client/maps.json')
serverMap = open(appDataPath + 'server/maps.json')

tempSuffix = "-TEM241P-I2-AM-TEMP"

newBuild = json.load(serverMap)
oldBuild : dict = None
if clientMap:
    oldBuild = json.load(clientMap)
    
blocks = dict()
def runFastOnFile(path) -> dict:
    fast = fastcdc(path, 65536 * .5, 65536, 65536 * 2, True, sha256)
    results = list(fast)
    idx = 0
    for result in results:
        blocks[result.hash] = result.data
        idx += 1
        
for file in newBuild["fileinfo"]:
    if oldBuild == None or (file not in oldBuild["fileinfo"] or newBuild["fileinfo"][file]["hash"] != oldBuild["fileinfo"][file]["hash"]):
        filesToPatch.append(file)

for file in filesToPatch:
    filePath = installDirectory + '/' + file
    if os.path.exists(filePath):
        runFastOnFile(filePath)

def resolve(block):
    if block["hash"] in blocks:
        return blocks[block["hash"]]
    else:
        return rwbin.rRequestFile(block["hash"])

for file in filesToPatch:  
    filePath = installDirectory + '/' + file + tempSuffix
    os.makedirs(os.path.dirname(filePath), exist_ok=True)
    if os.path.exists(filePath):
        os.remove(filePath)

    tempFile = open(filePath, 'wb')
    for block in newBuild["fileinfo"][file]["blocks"]:
        binData = resolve(newBuild["fileinfo"][file]["blocks"][block])
        tempFile.write(binData)
    tempFile.close()
        
for root, subdirs, files in os.walk(os.path.abspath(installDirectory)):
    for file in files:
        fileName = os.path.join(root, file)
        relName = os.path.relpath(fileName, installDirectory)
        relName = relName.replace(os.sep, '/')
        if file.endswith("-tem2345p"):
            name = os.path.join(root, file.replace("-tem2345p", ""))
            if os.path.exists(name):
                os.remove(name)
            os.rename(fileName, name)
        elif relName not in newBuild["fileinfo"]:
            os.remove(fileName)
    
    if not os.listdir(root):
        os.rmdir(root)
        
            
open(appDataPath + 'client/maps.json', 'w').write(str(json.dumps(newBuild, indent=4)))