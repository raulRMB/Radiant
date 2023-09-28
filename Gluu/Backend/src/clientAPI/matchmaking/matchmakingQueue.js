import Queue from "./queue.js"
import { randomUUID } from 'crypto'

export default class MatchmakingQueue {

    queues = {}
    lobbies = new Map()

    constructor(size) {
        this.matchSize = size
        for(let i = 0; i < size; i++) {
            this.queues.push(new Queue(i + 1))
        }
    }

    join(lobby) {
        const playersNeededForFullTeam = this.matchSize - lobby.players.length
        for(let i = playersNeededForFullTeam - 1; i >= 0; i--) {
            if(!this.queues[i].isEmpty) {
                const tmp = this.queues[i].dequeue()
                lobby.matchmakingTicket = tmp.id
                tmp.lobbies.push(lobby)
                this.queues[i + lobby.players.length].enqueue(tmp)
                this.lobbies[lobby.id] = i + lobby.players.length
                break
            }
            else if(i == 0) {
                const ticket = {
                    id: randomUUID(),
                    lobbies: [lobby]
                }
                this.lobbies[lobby.id] = lobby.players.length -1
                this.queues[lobby.players.length - 1].enqueue(ticket)
                lobby.matchmakingTicket = ticket.id
            }
        }
    }

    leave(lobby) {
        if(lobbies[lobby.id]) {
            this.
        }
    }

    getSubQueues() {
        return this.queues
    }

    mainQueue() {
        return this.queues[this.queues.length - 1]
    }

    getTeams() {
        const queue = this.mainQueue()
        console.log(`queue ${queue}`)
        if(queue.length >= 2) {
            return [queue.dequeue(), queue.dequeue()]
        } else {
            return null
        }
    }

}