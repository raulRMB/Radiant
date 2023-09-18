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
    removeFriend: 'removeFriend',
    notify: {
	    logout: 'logout',
	    cancelQueueResponse: 'cancelQueueResponse',
    	matchFound: 'matchFound',
        loginResponse: 'loginResponse',
        newFriendAdded: 'newFriendAdded',
        friendRemoved: 'friendRemoved',
        joinQueueResponse: 'joinQueueResponse',
        friendsStatusChanged: 'friendsStatusChanged',
        friendRequestReceived: 'friendRequestReceived'
    }
}