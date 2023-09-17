import {observable, makeObservable, action} from 'mobx'
import io from 'socket.io-client'
import sEvents from '../../../../socketEvents.mjs'

class Store {

    socket = null
    connected = false

    inQueue = false
    inMatch = false
    loggedIn = false

    notifications = []

    constructor() {
        if (import.meta.env.VITE_ENV === 'production') {
            this.socket = io(`https://${window.location.host}`, { transports: ['websocket'] });
        } else {
            this.socket = io(`http://localhost:3000`, { transports: ['websocket'] });
        }
        makeObservable(this, {
            inQueue: observable,
            loggedIn: observable,
            notifications: observable,
            setInQueue: action,
            setupSocketEvents: action,
            onConnected: action,
            onDisconneced: action,
            onLoginNotify: action,
            onLogoutNotify: action,
            onFriendRequestReceived: action,
            Logout: action
        })
        this.setupSocketEvents()
    }

    setupSocketEvents = () => {
        this.socket.on(sEvents.connect, () => this.onConnected());
        this.socket.on(sEvents.disconnect, () => this.onDisconneced());
        this.socket.on(sEvents.notify.loginResponse, () => this.onLoginNotify())
        this.socket.on(sEvents.notify.logout, () => this.onLogoutNotify())
        this.socket.on(sEvents.notify.friendRequestReceived, (msg) => this.onFriendRequestReceived(msg.from))
        this.socket.on(sEvents.notify.matchFound, (msg) => this.onMatchFound(msg))
        this.socket.on(sEvents.notify.joinQueueResponse, msg => this.joinQueueResponse(msg.success))
        this.socket.on(sEvents.notify.cancelQueueResponse, () => this.setInQueue(false))
    }

    setInQueue(val) {
        this.inQueue = val
    }

    onConnected() {
        console.log('connect')
        this.connected = true
    }

    onDisconneced() {
        console.log('disconnected')
        this.connected = false
        this.notifications = []
        this.loggedIn = false
    }

    onMatchFound(data) {
        this.inMatch = true
        //window.electron.ipcRenderer.send('matchFound', data);
    }

    onLogoutNotify() {
        this.notifications = []
        this.loggedIn = false
    }

    onFriendRequestReceived(from) {
        this.notifications.push({
            title: "Friend Request",
            message: `${from} sent you a friend request.`,
            from: from
        })
    }

    onLoginNotify() {
        this.loggedIn = true
    }

    Login = (loginData) => {
        this.socket.emit(sEvents.login, loginData)
    }

    Logout = () => {
        this.socket.emit(sEvents.logout)
    }

    JoinQueue = (lobbyData) => {
        this.socket.emit(sEvents.joinQueue, lobbyData)
    }

    joinQueueResponse = (success) => {
        if(success) {
            store.setInQueue(true)
        }
    }

    CancelQueue = () => {
        this.socket.emit(sEvents.cancelQueue)
    }

    SendFriendRequest = (username) => {
        this.socket.emit(sEvents.addFriend, {username})
    }

    AcceptFriendRequest = (username) => {
        this.socket.emit(sEvents.acceptFriendRequest, {username})
    }

}

const store = new Store()
const useStore = () => store

export default useStore