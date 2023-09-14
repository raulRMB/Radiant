import Queue from './queue.js'

const oneVone = new Queue(2)
const twoVtwo = new Queue(4)
const threeVthree = new Queue(6)

let serverManager;

function getQueue(queueName) {
    switch(queueName) {
      case '1v1':
        return oneVone;
      case '2v2':
        return twoVtwo;
      case '3v3':
        return threeVthree;
      default:
        return null;
    }
}

function checkForMatch(queue) {
    if(queue.length >= queue.matchSize) {
      const gameServer = serverManager.startMatch()
      if(gameServer) {
        for(let i = 0; i < queue.matchSize; i++) {
          const player = queue.dequeue()
          notifyPlayerOfMatch(player, gameServer)
        }
      }
    }
}

function notifyPlayerOfMatch(player, gameServer) {
    console.log(gameServer)
    player.emit('matchFound', {
      serverId: gameServer.name,
      port: gameServer.port,
      ip: gameServer.ip
    })
}

export default {
    joinQueue: (socket, queueName) => {
        const queue = getQueue(queueName)
        if(queue) {
          queue.enqueue(socket)
          checkForMatch(queue)
        }
    },
    setServerManager: (sm) => {
        serverManager = sm
    }
}