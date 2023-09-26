import docker from './docker.js'
import { networkInterfaces } from 'os'
const startPort = 8080
const servers = []
let ip = ''

  function updateServer(data) {
    const serverIndex = servers.findIndex(server => server.name == data.name)
    if(serverIndex != -1) {
      servers[serverIndex] = data;
    } else {
      servers.push(data);
    }
  }

  function updateStatus(name, status) {
    const serverIndex = servers.findIndex(server => server.name == name)
    if(serverIndex != -1) {
      servers[serverIndex].status = status;
    }
  }

  function getServerIndex() {
    let i = 0;
    while(servers.findIndex(server => server.name == `server-${i + 1}`) != -1) {
      i++;
    }
    return i;
  }

let serversStopped = 0
async function exitServer(server, signal) {
    const { err, stdout, stderr } = await docker.stopServer(server)
    if(err) {
      console.log(err)
    }
    serversStopped += 1
    console.log(`Servers Stopped: ${serversStopped} Remaining: ${servers.length - serversStopped}`)
    if(serversStopped >= servers.length) {
      console.log(`Done Shutting Down`)
      process.kill(process.pid, signal)
    }
}

const getIP = () => {
  const nets = networkInterfaces();
  const results = []
  for (const name of Object.keys(nets)) {
      for (const net of nets[name]) {
          const familyV4Value = typeof net.family === 'string' ? 'IPv4' : 4
          if (net.family === familyV4Value && !net.internal && name !== 'vEthernet (WSL)') {
              results.push(net.address);
          }
      }
  }
  return results[0]
}

const api = {

    add: async () => {
        const i = getServerIndex()
        const name = `server-${i + 1}`
        const port = i + startPort;
        updateServer({
          name: name,
          status: "propping",
          port: port,
          ip: ip
        })
        const containerExists = await docker.containerExists(name)
        if(containerExists) {
            await docker.restartServer(name)
        } else {
            await docker.runServer(name, port)
        }
        updateStatus(name, 'standby')
    },

    init: () => {
        ip = getIP()
    },

    addServers: async (num) => {
        for(let i = 0; i < num; i++) {
            api.add()
        }
    },
    
    remove: async (name) => {
        const index = servers.findIndex(server => server.name == name)
        if(index != -1) {
            const server = servers[index]
            updateStatus(name, 'stopping')
            await docker.stopServer(name)
            servers.splice(index, 1)
        }
    },
      
    restart: async (name) => {
        const index = servers.findIndex(server => server.name == name)
        if(index != -1) {
            const server = servers[index]
            updateStatus(name, 'restarting')
            await docker.restartServer(name)
            updateStatus(name, 'standby')
        }
    },

    stopAll: (signal) => {
        servers.forEach(server => exitServer(server.name, signal))
    },

    startMatch: () => {
        let server
        while(!server) {
            console.log('Match found, finding server.')
            server = servers.find(server => server.status == "standby")
        }
        server.status = "playing"
        docker.onStopped(server.name, () => {
            server.status = "standby"
        })
        return server
    },

    getServers: () => servers,
    count: () => servers.length
}

export default api;