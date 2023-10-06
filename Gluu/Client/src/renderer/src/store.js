import {observable, makeObservable, action} from 'mobx'
import io from 'socket.io-client'
import sEvents from '../../../../socketEvents.mjs'

class Store {

    socket = null
    connected = false

    inQueue = false
    inMatch = false
    loggedIn = false
    username = null

    patching = false
    patchingPercent = 0

    lobby = null

    notifications = []
    friends = []

    constructor() {
        if (import.meta.env.VITE_ENV === 'production') {
            this.socket = io(`ws://178.128.233.82:3000`, { transports: ['websocket'] });
        } else {
            this.socket = io(`ws://178.128.233.82:3000`, { transports: ['websocket'] });
        }
        makeObservable(this, {
            inQueue: observable,
            inMatch: observable,
            loggedIn: observable,
            lobby: observable,
            patching: observable,
            patchingPercent: observable,
            username: observable,
            notifications: observable,
            friends: observable,
            setupSocketEvents: action,
            updatePatchingPercent: action,
            onConnected: action,
            onDisconneced: action,
            joinQueueResponse: action,
            onMatchFound: action,
            checkForUpdate: action,
            onLoginNotify: action,
            onLogoutNotify: action,
            newFriendAdded: action,
            onFriendRemoved: action,
            AcceptFriendRequest: action,
            onCancelQueueResponse: action,
            onFriendRequestReceived: action,
            onLobbyInfo: action,
            onLobbyInvite: action,
            Logout: action
        })
        this.checkForUpdate()
        window.electron.ipcRenderer.on('patching-update', (event, data) => {
            if(data != 'DONE') {
                this.updatePatchingPercent(data)
            } else {
                this.patchingPercent = 100
                this.patching = false
            }
        });     
        this.setupSocketEvents()
    }

    updatePatchingPercent(data) {
        this.patchingPercent = data
    }

    registerUser(registerData) {
        this.socket.emit(sEvents.register, registerData)
    }

    checkForUpdate = async () => {
        const resp = await window.electron.ipcRenderer.invoke('get-version')
        console.log(`CDN VERSION: ${resp.remote} - LOCAL VERSION: ${resp.local}`)
        if(resp.remote != resp.local) {
            this.patching = true
            window.electron.ipcRenderer.send('update')
        } else {
            console.log('r')
        }
        console.log(resp)
    }

    setupSocketEvents = () => {
        this.socket.on(sEvents.connect, () => this.onConnected());
        this.socket.on(sEvents.disconnect, () => this.onDisconneced());
        this.socket.on(sEvents.notify.loginResponse, (msg) => this.onLoginNotify(msg))
        this.socket.on(sEvents.notify.logout, () => this.onLogoutNotify())
        this.socket.on(sEvents.notify.friendRequestReceived, (msg) => this.onFriendRequestReceived(msg))
        this.socket.on(sEvents.notify.matchFound, (msg) => this.onMatchFound(msg))
        this.socket.on(sEvents.notify.joinQueueResponse, msg => this.joinQueueResponse(msg.success))
        this.socket.on(sEvents.notify.cancelQueueResponse, () => this.onCancelQueueResponse())
        this.socket.on(sEvents.notify.newFriendAdded, (msg) => this.newFriendAdded(msg))
        this.socket.on(sEvents.notify.friendRemoved, (msg) => this.onFriendRemoved(msg.username))
        this.socket.on(sEvents.notify.friendsStatusChanged, (msg) => this.friendsStatusChanged(msg))
        this.socket.on(sEvents.notify.lobbyInfo, msg => this.onLobbyInfo(msg))
        this.socket.on(sEvents.notify.lobbyInviteReceived, msg => this.onLobbyInvite(msg))
    }

    onConnected() {
        console.log('connect')
        this.connected = true
    }

    onLobbyInvite(data) {
        console.log(data)
        this.notifications.push({
            title: "Invite",
            message: `${data.from} invited you to a lobby.`,
            lobbyId: data.lobbyId,
        })
    }

    newFriendAdded(friend) {
        console.log(friend)
        this.friends.push(friend)
    }

    friendsStatusChanged(msg) {
        const friend = this.friends.find(friend => friend.username === msg.username)
        if(friend) {
            friend.status = msg.status
        }
    }

    onDisconneced() {
        console.log('disconnected')
        this.connected = false
        this.inQueue = false
        this.inMatch = false
        this.notifications = []
        this.friends = []
        this.loggedIn = false
    }

    onLobbyInfo(data) {
        console.log(data)
        this.lobby = data
    }

    onMatchFound(data) {
        this.inMatch = true
        this.inQueue = false
        console.log('MATCH FOUND!')
        window.electron.ipcRenderer.send('matchFound', data);
    }

    onLogoutNotify() {
        this.notifications = []
        this.friends = []
        this.inQueue = false
        this.loggedIn = false
    }

    acceptLobbyInvite(lobbyId) {
        const index = this.notifications.findIndex(n => n.lobbyId === lobbyId && n.title === "Invite")
        if (index > -1) {
            this.notifications.splice(index, 1)
        }
        this.socket.emit(sEvents.acceptLobbyInvite, {lobbyId})
    }

    onCancelQueueResponse() {
        this.inQueue = false
    }

    onFriendRequestReceived(msg) {
        this.notifications.push({
            title: "Friend Request",
            message: `${msg.displayName} sent you a friend request.`,
            from: msg.from,
            username: msg.username 
        })
    }

    onFriendRemoved(username) {
        const index = this.friends.findIndex(n => n.username === username)
        if (index > -1) {
            this.friends.splice(index, 1)
        }
    }

    onLoginNotify(msg) {
        msg.friendsList.forEach(friend => {
            this.friends.push(friend)
        })
        msg.notifications.forEach(notification => {
            this.notifications.push({
                title: "Friend Request",
                message: `${notification.displayName} sent you a friend request.`,
                from: notification.displayName,
                username: notification.username
            })
        })
        this.username = msg.user
        this.loggedIn = true
    }

    Login = (loginData) => {
        this.socket.emit(sEvents.login, loginData)
    }

    Logout = () => {
        this.socket.emit(sEvents.logout)
    }

    JoinQueue = (lobbyData) => {
        this.socket.emit(sEvents.joinQueue, {selectionData: lobbyData, id: this.lobby.id})
    }

    joinQueueResponse = (success) => {
        if(success) {
            this.inQueue = true;
        }
    }

    CancelQueue = () => {
        this.socket.emit(sEvents.cancelQueue, {lobbyId: this.lobby.id})
    }

    SendFriendRequest = (username) => {
        this.socket.emit(sEvents.addFriend, {username})
    }

    AcceptFriendRequest = (username) => {
        const index = this.notifications.findIndex(n => n.username === username && n.title === "Friend Request")
        if (index > -1) {
            this.notifications.splice(index, 1)
        }
        this.socket.emit(sEvents.acceptFriendRequest, {username})
    }

    removeFriend = (username) => {
        this.socket.emit(sEvents.removeFriend, {username})
    }

    isLeader = () => {
        if(!this.lobby) {
            return false
        }
        return this.lobby.leader === this.username
    }

    sendInvite = (username) => {
        if(this.lobby) {
            this.socket.emit(sEvents.inviteToLobby, {username, lobbyId: this.lobby.id})
        }
    }
}

const store = new Store()
const useStore = () => store

export default useStore