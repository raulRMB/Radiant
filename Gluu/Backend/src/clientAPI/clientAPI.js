import sEvent from '../../../socketEvents.mjs'
import userManager from './users/users.js'
import queueManager from './matchmaking/queueManager.js'
import lobbies from './lobbies/lobbies.js'

const authMiddleware = (socket, userManager) => {
    socket.use((packet,next) => {
        const event = packet[0]
        if(event === sEvent.login || event === sEvent.disconnected || event === sEvent.register) {
          next()
        }
        else if(userManager.hasSession(socket)) {
          next()
        }
        else {
          console.log(`Event ${event} failed. Not Authorized.`)
          next(new Error('Not Authorized'))
        }
    })
}

export default (io) => {
  io.on(sEvent.connect, (socket) => {
      authMiddleware(socket, userManager)
      socket.on(sEvent.disconnect, (msg) => userManager.disconnect(socket, msg))
      socket.on(sEvent.login, (msg) => userManager.login(msg.username, msg.password, socket))
      socket.on(sEvent.logout, () => userManager.logout(socket, true))
      socket.on(sEvent.register, (msg) => userManager.registerUser(socket, msg))
      socket.on(sEvent.joinQueue, (msg) => queueManager.joinQueue(socket, msg))
      socket.on(sEvent.cancelQueue, (msg) => queueManager.leaveQueue(socket, msg.lobbyId))
      socket.on(sEvent.addFriend, (msg) => userManager.addFriend(socket, msg.username))
      socket.on(sEvent.acceptFriendRequest, msg => userManager.acceptFriend(socket, msg.username))
      socket.on(sEvent.removeFriend, msg => userManager.removeFriend(socket, msg.username))
      socket.on(sEvent.inviteToLobby, msg => lobbies.sendInviteToLobby(socket, msg.username, msg.lobbyId))
      socket.on(sEvent.acceptLobbyInvite, msg => lobbies.joinLobby(socket, msg.lobbyId))
      // socket.on(sEvent.addServer, async () => await serverManager.add())
      // socket.on(sEvent.restartServer, async (msg) => await serverManager.restart(msg.name))
      // socket.on(sEvent.removeServer, async (msg) => await serverManager.remove(msg.name))
  })
}