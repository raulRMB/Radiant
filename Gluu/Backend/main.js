import http from 'http'
import express from 'express'
import path from 'path'
import { fileURLToPath } from 'url'
import { Server } from 'socket.io'

//import nodeCleanup from 'node-cleanup';

//import serverManager from './src/servers/serverManager.js'
import cors from 'cors'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)
const port = 3000
//serverManager.init()
const app = express()
const server = http.createServer(app)
const io = new Server(server)

app.set('view engine', 'pug')
app.use("/socketio", express.static(path.join(__dirname, "node_modules/socket.io/client-dist")))
// app.use("/", express.static(path.join(__dirname, "static")))
app.use(cors({origin: '*'}))
//app.get('/', (req, res) => res.sendFile('index', {servers: serverManager.getServers()}))

//app.get('/download', (req, res) => res.sendFile('./data/client/Radiant-Setup.exe', { root: __dirname }))

import clientAPI from './src/clientAPI/clientAPI.js'
clientAPI(io)

import patching from './src/clientAPI/patching/patching.js'
patching(app, __dirname)

//serverManager.addServers(1)

//nodeCleanup(function (exitCode, signal) {
  // console.log('Stopping servers...')
  // if (signal) {
  //   if(serverManager.count() == 0) {
  //     process.kill(process.pid, signal)
  //   } else {
  //     serverManager.stopAll(signal)
  //     nodeCleanup.uninstall()
  //   }
  //   return false
  // }
//});

server.listen(port, () => console.log(`Running on port ${port}`))