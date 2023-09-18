export default {
    connect: 'connection',
    disconnect: 'disconnect',
    joinQueue: 'joinQueue',
    login: 'login',
    logout: 'logout',
    addFriend: 'addFriend',
    cancelQueue: 'cancelQueue',
    addServer: 'addServer',
    restartServer: 'restartServer',
    removeServer: 'removeServer',
    acceptFriendRequest: 'acceptFriendRequest',
    notify: {
	    logout: 'logout',
	    cancelQueueResponse: 'cancelQueueResponse',
    	matchFound: 'matchFound',
        loginResponse: 'loginResponse',
        newFriendAdded: 'newFriendAdded',
        joinQueueResponse: 'joinQueueResponse',
        friendRequestReceived: 'friendRequestReceived'
    }
}