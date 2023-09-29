import Queue from "./queue.js"
import { randomUUID } from 'crypto'

export default class MatchmakingQueue {

    queues = []
    total = 0
    lobbies = new Map()

    constructor(size) {
        this.teamSize = size
        for(let i = 0; i < size; i++) {
            this.queues.push(new Queue(i + 1))
        }
    }

    join(lobby) {
        const playersNeededForFullTeam = this.teamSize - lobby.players.length
        if(playersNeededForFullTeam === 0) {
            const ticket = {
                id: randomUUID(),
                lobbies: [lobby]
            }
            this.lobbies[lobby.id] = lobby.players.length -1
            this.queues[lobby.players.length - 1].enqueue(ticket)
            lobby.matchmakingTicket = ticket.id
            this.total += 1
            return
        }
        for(let i = playersNeededForFullTeam - 1; i >= 0; i--) {
            if(!this.queues[i].isEmpty) {
                const tmp = this.queues[i].dequeue()
                lobby.matchmakingTicket = tmp.id
                tmp.lobbies.push(lobby)
                this.queues[i + lobby.players.length].enqueue(tmp)
                tmp.lobbies.forEach(lb => this.lobbies[lb.id] = i + lobby.players.length)
                this.total += 1
                break
            }
            else if(i == 0) {
                const ticket = {
                    id: randomUUID(),
                    lobbies: [lobby]
                }
                this.lobbies[lobby.id] = lobby.players.length -1
                this.queues[lobby.players.length - 1].enqueue(ticket)
                this.total += lobby.players.length
                lobby.matchmakingTicket = ticket.id
            }
        }
    }

    leave(lobby) {
        if(this.lobbies[lobby.id] !== undefined) {
            let res = this.queues[this.lobbies[lobby.id]].remove(ticket => {
                let found = false
                ticket.lobbies.forEach(currLobby => {
                    if(currLobby.id === lobby.id) {
                        found = true
                    }
                })
                return found
            })
            if(res) {
                res.lobbies = res.lobbies.filter(currLobby => currLobby.id !== lobby.id)
                lobby.matchmakingTicket = null
                delete this.lobbies[lobby.id]
                let playerCount = 0
                res.lobbies.forEach(currLobby => {
                    playerCount += currLobby.players.length
                })
                if(playerCount > 0) {
                    this.queues[playerCount - 1].skip(res)
                    res.lobbies.forEach(lb => this.lobbies[lb.id] = playerCount - 1)
                }
                this.total -= lobby.players.length
            } else {
                console.log('Ticket not found!')
            }
        }
    }

    printSubQueues() {
        let i = 1
        this.queues.forEach((queue) => {
            console.log(`size ${i} has ${queue.length} tickets`)
            i += 1
        })
        console.log('\n')
    }

    getTeamSize() {
        return this.teamSize
    }

    isInQueue(lobby) {
        return this.lobbies[lobby.id] !== undefined
    }

    getSubQueues() {
        return this.queues
    }

    mainQueue() {
        return this.queues[this.queues.length - 1]
    }

    getTeams() {
        const queue = this.mainQueue()
        if(queue.length >= 2) {
            return [queue.dequeue(), queue.dequeue()]
        } else {
            return null
        }
    }

}