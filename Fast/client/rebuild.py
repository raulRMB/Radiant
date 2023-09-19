import os
import json
import rwbin

installDirectory = "C:/Radiant/Fast/client/build"
appDataPath = "C:/Radiant/Fast/"
clientMap : dict = None
oldBuildBlocks : dict = None
if os.path.exists(appDataPath + 'client/maps.json'):
    clientMap = open(appDataPath + 'client/maps.json')
serverMap = open(appDataPath + 'server/maps.json')

newBuild = json.load(serverMap)
if clientMap:
    oldBuild = json.load(clientMap)
    oldBuildBlocks = oldBuild["blocks"]

def resolve(block):
    if oldBuildBlocks and block["hash"] in oldBuildBlocks:
        return rwbin.rBinRange(oldBuildBlocks[block["hash"]], installDirectory, block["hash"])
    else:
        # we dont have this block, so we need to download it
        return rwbin.rRequestFile(block["hash"])

for file in newBuild["fileinfo"]:
    filePath = installDirectory + '/' + file + "-tem2345p"
    os.makedirs(os.path.dirname(filePath), exist_ok=True)
    if os.path.exists(filePath):
        os.remove(filePath)
    
    tempFile = open(filePath, 'wb')
    for block in newBuild["fileinfo"][file]:
        binData = resolve(newBuild["fileinfo"][file][block])
        tempFile.write(binData)
    tempFile.close()
        
for root, subdirs, files in os.walk(os.path.abspath(installDirectory)):
    for clientMap in files:
        fileName = os.path.join(root, clientMap)
        if clientMap.endswith("-tem2345p"):
            name = os.path.join(root, clientMap.replace("-tem2345p", ""))
            if os.path.exists(name):
                os.remove(name)
            os.rename(fileName, name)
        else:
            os.remove(fileName)
            
open(appDataPath + 'client/maps.json', 'w').write(str(json.dumps(newBuild, indent=4)))