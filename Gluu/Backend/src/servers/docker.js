import { exec } from 'child_process'
import util from 'util'
const cmd =  util.promisify(exec);

const api = {
      
    stopServer: async (name) => {
        const { err, stdout, stderr } = await cmd(`docker stop ${name}`)
        return { err, stdout, stderr }
    },
      
    restartServer: async (name) => {
        const { err, stdout, stderr } = await cmd(`docker restart ${name}`)
        return { err, stdout, stderr }
    },
      
    containerExists: async (name) => {
      try {
        const { err, stdout, stderr } = await cmd(`docker container inspect ${name}`)
        return stdout != '[]\n'
      } catch {
        return false;
      }
    },
      
    runServer: async (name, port) => {
        const { err, stdout, stderr } = await cmd(`docker run --name="${name}" --restart=always -p ${port}:7777/udp -td platformerserver:01`)
        if (err) {
          console.error(`Failed to start ${name} on port ${port}. ${err}`);
        }
        return {err, stdout, stderr }
    },
      
    onStopped: async (name, callback) => {
        await cmd(`docker wait ${name}`)
        callback()
    }
}

export default api;