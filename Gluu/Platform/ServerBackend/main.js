
import http from 'http'
import { Server } from 'socket.io'
import express from 'express'
import path from 'path'
import { fileURLToPath } from 'url';
import nodeCleanup from 'node-cleanup';
import util from './src/util/util.js'

import serverManager from './src/servers/serverManager.js'
import queueManager from './src/matchmaking/queueManager.js'
import userManager from './src/db/db.js'
import sEvent from '../socketEvents.mjs'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)
const port = 3000
serverManager.init()
const app = express()
const server = http.createServer(app)
const io = new Server(server)

app.set('view engine', 'pug')
app.use("/socketio", express.static(path.join(__dirname, "node_modules/socket.io/client-dist")))
app.get('/', (req, res) => res.render('index', {servers: serverManager.getServers()}))

io.on(sEvent.connect, (socket) => {
  util.authMiddleware(socket, userManager)
  socket.on(sEvent.disconnect, (reason) => {
    const user = userManager.getSessionUser(socket)
    if(user) {
      console.log(`${user.username} disconnected - ${reason}`)
    }
  })
  socket.on(sEvent.login, (msg) => userManager.login(msg.username, msg.password, socket))
  socket.on(sEvent.logout, () => userManager.logout(socket))
  socket.on(sEvent.joinQueue, (msg) => queueManager.joinQueue(socket, msg.queue))
  socket.on(sEvent.addServer, async () => await serverManager.add())
  socket.on(sEvent.restartServer, async (msg) => await serverManager.restart(msg.name))
  socket.on(sEvent.removeServer, async (msg) => await serverManager.remove(msg.name))
  socket.on(sEvent.cancelQueue, () => queueManager.leaveQueue(socket))
  socket.on(sEvent.addFriend, (msg) => userManager.addFriend(socket, msg.username))
})

queueManager.setReferences(serverManager, userManager)
userManager.setReferences(queueManager)
//serverManager.addServers(1)

nodeCleanup(function (exitCode, signal) {
  console.log('Stopping servers...')
  if (signal) {
    if(serverManager.count() == 0) {
      process.kill(process.pid, signal)
    } else {
      serverManager.stopAll(signal)
      nodeCleanup.uninstall()
    }
    return false
  }
});

server.listen(port, () => console.log(`Running on port ${port}`))