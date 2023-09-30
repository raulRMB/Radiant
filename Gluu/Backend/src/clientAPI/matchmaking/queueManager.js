import sEvent from '../../../../socketEvents.mjs'
import userManager from '../users/users.js'
import MatchmakingQueue from './matchmakingQueue.js'
import lobbies from '../lobbies/lobbies.js'
import { randomUUID } from 'crypto'

const oneVone = new MatchmakingQueue(1)
const twoVtwo = new MatchmakingQueue(2)
const threeVthree = new MatchmakingQueue(3)

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
    const teams = queue.getTeams()
    if(teams != null) {
      const gameServer = {
        name: 'server-1',
        port: 7777,
        ip: '178.128.233.82'
      }//serverManager.startMatch()
      const newMatch = {
        id: randomUUID(),
        teams 
      }
      // check for valid game server
      if(true) {
        teams.forEach(team => {
          console.log(team)
          team.lobbies.forEach(lobby => {
            lobby.players.forEach(player => {
              const playerSocket = userManager.getSocketFromUsername(player.username)
              notifyPlayerOfMatch(playerSocket, gameServer)
            })
          })
        })
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
        const queue = getQueue(queueString)
        if(lobby.players.length > queue.getTeamSize()) {
          socket.emit(sEvent.notify.joinQueueResponse, {success: false, message: "Too many players for queue"})
          return
        }
        if(user && queue && !inQueue.has(lobby.id)) {
          lobby.queueType = queueString
          inQueue.set(lobby.id, queueString)
          queue.join(lobby)
          //console.log(`${lobby.id} joined queue - ${queueString}`)
          socket.emit(sEvent.notify.joinQueueResponse, {success: true})
          checkForMatch(queue)
        }
    },
    leaveQueue: (socket, lobbyId) => {
      const user = userManager.getSessionUser(socket)
      const lobby = lobbies.getLobby(lobbyId)
      if(!lobby) {
        return
      }
      let playerInLobby = false
      lobby.players.forEach(player => {
        if(player.username === user.username) {
          playerInLobby = true
        }
      })
      if(!playerInLobby) {
        return
      }
      const queue = getQueue(lobby.queueType)
      if(user && inQueue.has(lobby.id) && queue) {
        queue.leave(lobby)
        inQueue.delete(lobby.id)
        //console.log(`${lobby.id} left queue`)
        socket.emit(sEvent.notify.cancelQueueResponse, {success: true})
      }
    },
}