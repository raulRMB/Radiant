import Database from 'better-sqlite3'
import { randomBytes, scrypt } from 'crypto'
import queueManager from '../matchmaking/queueManager.js'
import lobbies from '../lobbies/lobbies.js'
import sEvents from '../../../../socketEvents.mjs'
const db = new Database('./data/database.db')
db.pragma('journal_mode = WAL')

db.exec('CREATE TABLE IF NOT EXISTS users(username UNIQUE, password_hash UNIQUE, salt UNIQUE, displayName)')
db.exec('CREATE TABLE IF NOT EXISTS friendrequests(sender, recipient, active)')
db.exec('CREATE TABLE IF NOT EXISTS friends(user1, user2, PRIMARY KEY (user1, user2))')
const register = db.prepare('INSERT INTO users (username, password_hash, salt, displayName) VALUES (@username, @password_hash, @salt, @displayName)')

const createFriendRequest = db.prepare('INSERT INTO friendrequests (sender, recipient, active) VALUES (@sender, @recipient, @active)')
const areFriends = db.prepare('SELECT * FROM friends WHERE user1 = ? AND user2 = ?')
const getFriendRequests = db.prepare('SELECT users.username, users.displayName AS displayName FROM friendrequests INNER JOIN users ON friendrequests.sender = users.username WHERE recipient = ? AND active = 1')

const userPS = db.prepare('SELECT * FROM users WHERE username = ?')
const findActiveInvite = db.prepare('SELECT * FROM friendrequests WHERE sender = ? AND recipient = ? AND active = 1')
const acceptFriendRequest = db.prepare('INSERT INTO friends (user1, user2) VALUES (@user1, @user2)')
const deactivateInvite = db.prepare('UPDATE friendrequests SET active = 0 WHERE sender = ? AND recipient = ? AND active = 1')
const removeFriend = db.prepare('DELETE FROM friends WHERE user1 = ? AND user2 = ?')
const getFriends = db.prepare('SELECT users.username, users.displayName AS displayName FROM friends INNER JOIN users ON friends.user2 = users.username WHERE friends.user1 = ?')

let sessionsIdToUser = {}
let sessionsUsernameToSocket = {}

const api = {
    registerUser: (username, password) => {
        const salt = randomBytes(40)
        scrypt(password, salt, 64, (err, derived_key) => {
            if(!err) {
                //db.transaction(() => {
                    const upperCaseUsername = username.toUpperCase()
                    register.run({username: upperCaseUsername, password_hash: derived_key, salt, displayName: username})
                //})
            }
        })
    },
    disconnect: (socket, reason) => {
        const user = api.getSessionUser(socket)
        api.logout(socket, false)
        if(user) {
          console.log(`${user.username} disconnected - ${reason}`)
        }
    },
    login: (username, password, socket) => {
        const user = userPS.get(username.toUpperCase())
        if(user) {
            scrypt(password, user.salt, 64, (err, derived_key) => {
                if(!err) {
                    const validPass = derived_key.equals(user.password_hash)
                    console.log(validPass ? `${username} logged in` : `login attempt failed for: ${username}`)
                    if(validPass) {
                        const existingSocket = sessionsUsernameToSocket[user.username]
                        if(existingSocket) {
                            api.logout(existingSocket, true)
                        }
                        user.socket = socket
                        sessionsIdToUser[socket.id] = user
                        sessionsUsernameToSocket[user.username] = socket
                        const friendRequests = getFriendRequests.all(user.username)
                        const loginData = {
                            notifications: friendRequests,
                            friendsList: getFriends.all(user.username),
                            user: user.username
                        }
                        loginData.friendsList.forEach(friend => {
                            const friendsSocket = api.getSocketFromUsername(friend.username)
                            friend.status = friendsSocket === undefined ? 'Offline' : 'Online'
                            if(friendsSocket) {
                                friendsSocket.emit(sEvents.notify.friendsStatusChanged, {
                                    username: user.username,
                                    status: 'Online',
                                    displayName: user.displayName
                                })
                            }
                        })
                        socket.emit(sEvents.notify.loginResponse, loginData)
                        const lobbyId = lobbies.createLobby(socket)
                        lobbies.notifyLobbyUpdate(lobbyId)
                    }
                }
            })
        }
    },
    changeUserStatus: (socket, status) => {
        const user = api.getSessionUser(socket)
        if(user) {
            const friends = getFriends.all(user.username)
            friends.forEach(friend => {
                const friendsSocket = api.getSocketFromUsername(friend.username)
                if(friendsSocket) {
                    friendsSocket.emit(sEvents.notify.friendsStatusChanged, {
                        username: user.username,
                        status: status,
                        displayName: user.displayName
                    })
                }
            })
        }
    },
    logout: (socket, notify) => {
        if(notify) {
            socket.emit(sEvents.notify.logout)
        }
        api.changeUserStatus(socket, 'Offline')
        queueManager.leaveQueue(socket)
        const user = api.getSessionUser(socket)
        if(user) {
            if(user.lobbyId) {
                lobbies.leaveLobby(user.lobbyId, user)
            }
            delete sessionsUsernameToSocket[user.username]
            delete sessionsIdToUser[socket.id]
        }
    },
    addFriend: (socket, username) => {
        const recipient = userPS.get(username.toUpperCase())
        const sender = api.getSessionUser(socket)
        if(recipient) {
            if(recipient.username === sender.username) {
                return
            }
            const activeInvite = findActiveInvite.get(sender.username, recipient.username)
            const hasFriend = areFriends.get(sender.username, recipient.username)
            if(!activeInvite && !hasFriend) {
                createFriendRequest.run({sender: sender.username, recipient: recipient.username, active: 1})
                const recipientSocket = api.getSocketFromUsername(recipient.username)
                if(recipientSocket) {
                    recipientSocket.emit(sEvents.notify.friendRequestReceived, {username: sender.username, displayName: sender.displayName})
                }
            }
        }
    },
    acceptFriend: (socket, username) => {
        if(username) {
            const sender = userPS.get(username.toUpperCase())
            const recipient = api.getSessionUser(socket)
            console.log(sender)
            console.log(recipient)
            const activeInvite = findActiveInvite.get(sender.username, recipient.username)
            if(activeInvite) {
                acceptFriendRequest.run({user1: sender.username, user2: recipient.username})
                acceptFriendRequest.run({user1: recipient.username, user2: sender.username})
                deactivateInvite.run(sender.username, recipient.username)
                const friendThatSentRequestSocket = api.getSocketFromUsername(sender.username)
                socket.emit(sEvents.notify.newFriendAdded, {username: sender.username, displayName: sender.displayName, status: friendThatSentRequestSocket ? 'Online' : 'Offline'})
                if(friendThatSentRequestSocket) {
                    friendThatSentRequestSocket.emit(sEvents.notify.newFriendAdded, {username: recipient.username, displayName: recipient.displayName, status: 'Online'})
                }
            }
        }
    },
    removeFriend: (socket, username) => {
        const f1 = userPS.get(username.toUpperCase())
        const f2 = api.getSessionUser(socket)
        removeFriend.run(f1.username, f2.username)
        removeFriend.run(f2.username, f1.username)
        socket.emit(sEvents.notify.friendRemoved, {username: f1.username})
        const friendSocket = api.getSocketFromUsername(f1.username)
        if(friendSocket) {
            friendSocket.emit(sEvents.notify.friendRemoved, {username: f2.username})
        }
    },
    getUserFromUsername: (username) => {return userPS.get(username.toUpperCase())},
    getSessionUser: (socket) => sessionsIdToUser[socket.id],
    getSocketFromUsername: (username) => sessionsUsernameToSocket[username],
    hasSession: (socket) => sessionsIdToUser[socket.id] !== undefined,
}

const initTestData = () => {
    api.registerUser('mash', 'test123')
    api.registerUser('mash2', 'test123')
    api.registerUser('mash3', 'test123')
    api.registerUser('mash4', 'test123')
}
//initTestData()

export default api