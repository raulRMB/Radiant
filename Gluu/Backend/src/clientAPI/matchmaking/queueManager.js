import sEvent from '../../../../socketEvents.mjs'
import userManager from '../users/users.js'
import MatchmakingQueue from './matchmakingQueue.js'
import lobbies from '../lobbies/lobbies.js'
import { randomUUID } from 'crypto'

const oneVone = new MatchmakingQueue(2)
const twoVtwo = new MatchmakingQueue(4)
const threeVthree = new MatchmakingQueue(6)

const inQueue = new Map()

const match = new Map()
const inMatch = new Map()

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
    teams = queue.getTeamsForMatch()
    if(teams != null) {
      const gameServer = serverManager.startMatch()
      const newMatch = {
        id: randomUUID(),
        teams 
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
    joinQueue: (socket, data) => {
        const queueString = data.selectionData.queue 
        const lobby = lobbies.getLobby(data.id) 
        const user = userManager.getSessionUser(socket)
        if(user.username !== lobby.leader) {
          return // someone other than leader is attempting to fuck with the lobby
        }
        const queue = getQueue(queueName)
        if(user && queue && !inQueue.has(lobby.id)) {
          inQueue.set(lobby.id, queueString)
          queue.join(lobby)
          console.log(`${lobby.id} joined queue - ${queueName}`)
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
}