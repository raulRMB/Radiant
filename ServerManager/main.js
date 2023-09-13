
import http from 'http'
import { Server } from 'socket.io'
import express from 'express'
import path from 'path'
import { fileURLToPath } from 'url';
import nodeCleanup from 'node-cleanup';

import serverManager from './src/servers/serverManager.js'
import queueManager from './src/matchmaking/queueManager.js'

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

io.on('connection', (socket) => {
  console.log("Connected")
  socket.on("disconnect", (reason) => {
    console.log(`Disconnected: ${reason}`)
  });
  socket.on('joinQueue', (msg) => queueManager.joinQueue(socket, msg.queue))
  socket.on('addServer', async () => await serverManager.add())
  socket.on('restartServer', async (msg) => await serverManager.restart(msg.name))
  socket.on('removeServer', async (msg) => await serverManager.remove(msg.name))
})

queueManager.setServerManager(serverManager)
serverManager.addMulti(5)

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