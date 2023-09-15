import Database from 'better-sqlite3'
import { randomBytes, scrypt } from 'crypto'
import sEvents from '../../../socketEvents.mjs'
const db = new Database('./data/database.db')
db.pragma('journal_mode = WAL')

db.exec('CREATE TABLE IF NOT EXISTS users(username UNIQUE, password_hash UNIQUE, salt UNIQUE)')
const register = db.prepare('INSERT INTO users (username, password_hash, salt) VALUES (@username, @password_hash, @salt)')

const userPS = db.prepare('SELECT * FROM users WHERE username = ?')

let queueManager

let sessionsIdToUser = {}
let sessionsUsernameToSocket = {}

const api = {
    registerUser: (username, password) => {
        const salt = randomBytes(40)
        scrypt(password, salt, 64, (err, derived_key) => {
            if(!err) {
                //db.transaction(() => {
                    register.run({username, password_hash: derived_key, salt})
                //})
            }
        })
    },
    login: (username, password, socket) => {
        const user = userPS.get(username)
        if(user) {
            scrypt(password, user.salt, 64, (err, derived_key) => {
                if(!err) {
                    const validPass = derived_key.equals(user.password_hash)
                    console.log(validPass ? `${username} logged in` : `login attempt failed for: ${username}`)
                    if(validPass) {
                        const existingSocket = sessionsUsernameToSocket[user.username]
                        if(existingSocket) {
                            existingSocket.emit(sEvents.notify.logout)
                            queueManager.leaveQueue(existingSocket)
                        }
                        user.socket = socket
                        sessionsIdToUser[socket.id] = user
                        sessionsUsernameToSocket[user.username] = socket
                        socket.emit(sEvents.notify.loginResponse)
                    }
                }
            })
        }
    },
    getSessionUser: (socket) => sessionsIdToUser[socket.id],
    getSocketFromUsername: (username) => sessionsUsernameToSocket[username],
    hasSession: (socket) => sessionsIdToUser[socket.id] !== undefined,
    setReferences: (qm) => {
        queueManager = qm
    }
}

//api.registerUser('mash2', 'test123')

export default api