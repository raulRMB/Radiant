import { networkInterfaces } from 'os'
import sEvent from '../../../socketEvents.mjs'

export default {
    getIP: () => {
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
    },
    authMiddleware: (socket, userManager) => {
        socket.use((packet,next) => {
            const event = packet[0]
            if(event === sEvent.login || event === sEvent.disconnected) {
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
}