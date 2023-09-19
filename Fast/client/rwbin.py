import os

blockCache = dict()

def rBinRange(blockInfo, installDirectory, blockHash):
    if blockHash in blockCache:
        return blockCache[blockHash]
    with open(installDirectory + '/' + blockInfo[0], 'rb') as file:
        bts = file.read(-1)
        data = bts[blockInfo[2] : blockInfo[2] + blockInfo[3]]
        blockCache[blockHash] = data
        return data
    
def rRequestFile(hash):
    with open("C:/Radiant/Fast/server/outputs/" + hash, 'rb') as file:
        return file.read(-1)
    
