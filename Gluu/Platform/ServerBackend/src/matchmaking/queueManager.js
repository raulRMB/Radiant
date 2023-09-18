import Queue from './queue.js'
import sEvent from '../../../socketEvents.mjs'
import { randomUUID } from 'crypto'

const oneVone = new Queue(2)
const twoVtwo = new Queue(4)
const threeVthree = new Queue(6)

const inQueue = new Map()

const match = new Map()
const inMatch = new Map()

let serverManager;
let userManager;

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
      const newMatch = {
        id: randomUUID(),
        teams: [] 
      }
      if(gameServer) {
        for(let i = 0; i < queue.matchSize; i++) {
          const playerSocket = userManager.getSocketFromUsername(queue.dequeue())
          notifyPlayerOfMatch(playerSocket, gameServer)
        }
      }
    }
}

function notifyPlayerOfMatch(playerSocket, gameServer) {
    playerSocket.emit(sEvent.notify.matchFound, {
      serverId: gameServer.name,
      port: gameServer.port,
      ip: gameServer.ip
    })
}

export default {
    joinQueue: (socket, queueName) => {
        const queue = getQueue(queueName)
        const user = userManager.getSessionUser(socket)
        if(user && queue && !inQueue.has(user.username)) {
          inQueue.set(user.username, queueName)
          queue.enqueue(user.username)
          console.log(`${user.username} joined queue - ${queueName}`)
          socket.emit(sEvent.notify.joinQueueResponse, {success: true})
          checkForMatch(queue)
        }
    },
    leaveQueue: (socket) => {
      const user = userManager.getSessionUser(socket)
      if(user && inQueue.has(user.username)) {
        const queue = getQueue(inQueue.get(user.username))
        queue.remove(user.username)
        inQueue.delete(user.username)
        console.log(`${user.username} left queue`)
        socket.emit(sEvent.notify.cancelQueueResponse, {success: true})
      }
    },
    setReferences: (sm, um) => {
        serverManager = sm
        userManager = um
    }
}