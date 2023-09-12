
import { exec } from 'child_process'
import Queue from './queue.js'
import http from 'http'
import { Server } from 'socket.io'
import express from 'express'
import nodeCleanup from 'node-cleanup';

const app = express()
const server = http.createServer(app)
const io = new Server(server)
const port = 3000

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html')
})

io.on('connection', (socket) => {
  console.log('a user connected')
  socket.on('joinQueue', (msg) => {
    console.log(msg)
    joinQueue(socket, msg.queue)
  });
})
let shuttingDown = false;
const standbyServerCount = 5;
const startPort = 8080
const servers = []
const oneVone = new Queue(2)
const twoVtwo = new Queue(4)
const threeVthree = new Queue(6)

for(let i = 0; i < standbyServerCount; i++) {
  initStandbyServer(i);
}

function getQueue(queueName) {
  switch(queueName) {
    case '1v1':
      return oneVone;
    case '2v2':
      return twoVtwo;
    case '3v3':
      return threeVthree;
    default:
      return oneVone;
  }
}

function joinQueue(socket, queueName) {
  const queue = getQueue(queueName)
  queue.enqueue(socket)
  checkForMatch(queue)
}

function findOpenServer() {
  return servers.find(server => {
    console.log(server)
    return server.status == "standby"
  })
}

function notifyPlayerOfMatch(player, gameServer) {
  console.log(`notify ${player} of their match on ${gameServer.name}`)
  player.emit('matchFound', {
    serverId: gameServer.name,
    port: gameServer.port
  })
}

function checkForMatch(queue) {
  if(queue.length >= queue.matchSize) {
    const gameServer = findOpenServer()
    if(gameServer) {
      gameServer.status = "playing"
      for(let i = 0; i < queue.matchSize; i++) {
        const player = queue.dequeue()
        notifyPlayerOfMatch(player, gameServer)
      }
    }
  }
}

function initStandbyServer(i) {
  const name = `server-${i + 1}`
  const port = i + startPort;
  exec(`docker run --name="${name}" -p ${port}:7777/udp -td radiant:01`, (err, stdout, stderr) => {
    const containerHash = stdout;
    if (err) {
      console.error(`Failed to start ${name} on port ${port}. ${err}`);
      return;
    }
    exec(`docker wait ${containerHash}`, (err, stdout, stderr) => {
      exec(`docker rm ${containerHash}`, (err, stdout, stderr) => {
        if (err) {
          return;
        }
        if(!shuttingDown) {
          initStandbyServer(i)
        }
      })
    })
    servers.push({
      id: containerHash,
      name: name,
      status: "standby",
      port: port
    });
    console.log(`Started ${name} on port ${port}`);
  });
}

let serversShutdown = 0
function exitServer(server, signal) {
  exec(`docker stop ${server}`, (err, stdout, stderr) => {
    exec(`docker rm ${server}`, (err, stdout, stderr) => {
      serversShutdown += 1
      console.log(`Servers Shutdown: ${serversShutdown} Remaining: ${standbyServerCount - serversShutdown}`)
      if(serversShutdown >= standbyServerCount) {
        console.log(`Done Shutting Down`)
        process.kill(process.pid, signal)
      }
    })
  })
}

nodeCleanup(function (exitCode, signal) {
  console.log('Shutting down...')
  if (signal) {
    shuttingDown = true
    servers.forEach(server => exitServer(server.id, signal))
    nodeCleanup.uninstall()
    return false
  }
});

server.listen(port, () => console.log(`Running on port ${port}`))