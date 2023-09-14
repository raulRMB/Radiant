import Database from 'better-sqlite3'
import { randomBytes, scrypt } from 'crypto'
const db = new Database('./data/database.db')
db.pragma('journal_mode = WAL')

db.exec('CREATE TABLE IF NOT EXISTS users(username UNIQUE, password_hash UNIQUE, salt UNIQUE)')
const register = db.prepare('INSERT INTO users (username, password_hash, salt) VALUES (@username, @password_hash, @salt)')

const userPS = db.prepare('SELECT * FROM users WHERE username = ?')

let sessions = {}

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
                    console.log(validPass ? 'logged in' : 'login failed')
                    if(validPass) {
                        sessions[socket.id] = user;
                        socket.emit('loggedIn')
                    }
                }
            })
        }
    },
    getSessionUser: (socket) => sessions[socket.id],
    hasSession: (socket) => sessions[socket.id] !== undefined
}

//api.registerUser('mash', 'test123')

export default api