import { randomUUID } from 'crypto'
import sEvents from '../../../socketEvents.mjs'
import userManager from '../users/users.js'

let lobbies = {}

const api = {
    createLobby: (socket) => {
        const user = userManager.getSessionUser(socket)
        const id = randomUUID()
        lobbies[id] = {
            openLobby: false,
            id: id,
            players: [{
                username: user.username,
                displayName: user.displayName
            }],
            leader: user.username
        }
        user.lobbyId = id
        return id
    },
    joinLobby: (socket, id) => {
        const user = userManager.getSessionUser(socket)
        const lobby = lobbies[id]
        lobby.players.push({
            username: user.username,
            displayName: user.displayName
        })
        api.leaveLobby(user.lobbyId, user)
        user.lobbyId = id
        api.notifyLobbyUpdate(lobby.id)
    },
    notifyLobbyUpdate: (id) => {
        const lobby = lobbies[id]
        lobby.players.forEach(player => {
            const socket = userManager.getSocketFromUsername(player.username)
            socket.emit(sEvents.notify.lobbyInfo, lobby)
        })
    },
    sendInviteToLobby: (socket, username, lobbyId) => {
        const from = userManager.getSessionUser(socket)
        const user = userManager.getUserFromUsername(username)
        const lobby = lobbies[lobbyId]
        if(user && lobby) {
            const socket = userManager.getSocketFromUsername(user.username)
            if(socket) {
                socket.emit(sEvents.notify.lobbyInviteReceived, {from: from.displayName, lobbyId: lobby.id})
            }
        }
    },
    getLobby: (id) => {
        return lobbies[id]
    },
    leaveLobby: (id, user) => {
        if(lobbies[id].players.length === 1) {
            delete lobbies[id]
        } else {
            let isLeader;
            const index = lobbies[id].players.findIndex(player => player.username === user.username)
            if (index > -1) {
                isLeader = lobbies[id].players[index].username === lobbies[id].leader
                lobbies[id].players.splice(index, 1)
            }
            if(isLeader) {
                lobbies[id].leader = lobbies[id].players[0].username
            }
            api.notifyLobbyUpdate(id)
        }
    },
}

export default api