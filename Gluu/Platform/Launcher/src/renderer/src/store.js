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
    friends = []

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
            friends: observable,
            setupSocketEvents: action,
            onConnected: action,
            onDisconneced: action,
            joinQueueResponse: action,
            onMatchFound: action,
            onLoginNotify: action,
            onLogoutNotify: action,
            newFriendAdded: action,
            AcceptFriendRequest: action,
            onCancelQueueResponse: action,
            onFriendRequestReceived: action,
            Logout: action
        })
        this.setupSocketEvents()
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
        this.socket.on(sEvents.notify.friendsStatusChanged, (msg) => this.friendsStatusChanged(msg))
    }

    onConnected() {
        console.log('connect')
        this.connected = true
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
        this.notifications = []
        this.friends = []
        this.loggedIn = false
    }

    onMatchFound(data) {
        this.inMatch = true
        //window.electron.ipcRenderer.send('matchFound', data);
    }

    onLogoutNotify() {
        this.notifications = []
        this.friends = []
        this.loggedIn = false
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
            this.inQueue = true;
        }
    }

    CancelQueue = () => {
        this.socket.emit(sEvents.cancelQueue)
    }

    SendFriendRequest = (username) => {
        this.socket.emit(sEvents.addFriend, {username})
    }

    AcceptFriendRequest = (username) => {
        console.log(username)
        const index = this.notifications.findIndex(n => n.username === username && n.title === "Friend Request")
        if (index > -1) {
            this.notifications.splice(index, 1)
        }
        this.socket.emit(sEvents.acceptFriendRequest, {username})
    }
}

const store = new Store()
const useStore = () => store

export default useStore