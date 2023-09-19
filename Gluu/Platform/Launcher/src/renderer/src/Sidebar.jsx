import React, { useEffect, useLayoutEffect, useRef, useState } from 'react';
import { FaCaretLeft, FaCaretRight, FaUsers, FaBell, FaCheck } from 'react-icons/fa';
import { FaXmark } from 'react-icons/fa6'
import useWindowDimensions from './useWindowDimensions';
import useStore from './store';
import usePopup from './usePopup';
import { observer } from 'mobx-react';

const FriendCard = ({username, status, displayName}) => {
    const store = useStore()
    const contextMenu = useRef(null)
    const [dimensions, setDimensions] = useState({ width: 0, height: 0 });
    const [open, setOpen] = useState(false);
    const [position, setPosition] = useState({x: 0, y: 0});
    const [clickPosition, setClickPosition] = useState({x: 0, y: 0});
    const { height, width } = useWindowDimensions();
    useLayoutEffect(() => {
        const click = {x: clickPosition.x * width, y: clickPosition.y * height}
        const x = click.x + dimensions.width > width ? width - dimensions.width : click.x
        const y = click.y + dimensions.height > height ? height - dimensions.height : click.y
        setPosition({x, y})
    }, [clickPosition, dimensions, width, height])
    useLayoutEffect(() => {
        if (contextMenu.current) {
            setDimensions({
              width: contextMenu.current.offsetWidth,
              height: contextMenu.current.offsetHeight
            });
        }
      }, [contextMenu, open]);
    useEffect(() => {
        const handleClick = (e) => {
            if(!e.target.classList.contains('menuButton')) {
                setOpen(false)
            }
        }
        document.addEventListener("mousedown", handleClick);
        return () => document.removeEventListener("mousedown", handleClick);
    }, [open]);
    const getColorFromStatus = () => {
        switch(status) {
            case 'Online': return 'bg-green-200'
            case 'Offline': return 'bg-gray-300'
            case 'Away': return 'bg-red-400'
            case 'In Game': return 'bg-blue-400'
        }
    }
    return (
        <div>
            <div onContextMenu={(e) => {
                e.preventDefault()
                setClickPosition({x: e.pageX / width, y: e.pageY / height})
                setOpen(true)
            }} className="w-full bg-gray-600 border border-1 px-3 mb-1 py-4 text-white hover:bg-gray-700 border-slate-600">
                <div className="flex flex-row items-center justify-left">
                    <div className={`rounded-full ${getColorFromStatus()} w-3 h-3 mx-1`}></div>
                    <h3>{displayName}</h3>
                </div>
            </div>
            <div ref={contextMenu} className={`${!open ? 'hidden' : ''} fixed text-sm whitespace-nowrap bg-gray-600 border border-gray-800`} style={{top: position.y, left: position.x}}>
                <ul>
                    <li onClick={() => {
                        store.sendInvite(username)
                        setOpen(false)
                    }} className='menuButton py-2 px-6 border-gray-700 border-b hover:bg-gray-800 hover:text-white'>Invite To Game</li>
                    <li onClick={() => {
                        store.removeFriend(username)
                        setOpen(false)
                    }} className='menuButton py-2 px-6 border-gray-700 border-b hover:bg-gray-800 hover:text-white'>Remove Friend</li>
                </ul>
            </div>
        </div>
    )
}

const FriendsList = observer(() => {
    const friends = []
    const store = useStore()
    store.friends.forEach(friend => {
        friends.push(<FriendCard username={friend.username} displayName={friend.displayName} status={friend.status}/>)
    })
    friends.sort((a, b) => {
        const x = a.props.status === 'Online' ? 1 : -1
        const y = b.props.status === 'Online' ? 1 : -1
        return y - x
    })
    return (
        <div className='w-full'>
            {friends}
        </div>
    )
})

const buildNotification = (notification) => {
    if(notification.title === "Friend Request") {
        return <FriendNotification 
        title={notification.title} 
        message={notification.message}
        from={notification.from}
        username={notification.username}
    />
    }
    else if(notification.title === "Invite") {
        return <InviteNotification 
        title={notification.title} 
        message={notification.message}
        lobbyId={notification.lobbyId}
    />
    }
}

const Notifications = observer(() => {
    const notifications = []
    const store = useStore()
    store.notifications.forEach(notification => {
        notifications.push(buildNotification(notification))
    })
    return (
        <div className='w-full'>
            {notifications}
        </div>
    )
})

const InviteNotification = ({title, message, lobbyId}) => {
    const store = useStore()
    return (
        <div className="w-full bg-gray-600 border border-1 mb-1 px-3 py-4 text-white border-slate-600">
            <div className="flex text-left flex-col items-center justify-left">
                <h3 className="text-slate-100 font-bold text-lg">{title}</h3>
                <p>{message}</p>
                <div className='w-full flex flex-row justify-around'>
                    <button onClick={() => {
                      store.acceptLobbyInvite(lobbyId)
                    }} className={`rounded-md w-1/2 p-3 flex justify-center hover:bg-gray-700 items-center border border-gray-600 bg-gray-800`}>
                        <FaCheck size={18} color='green'/>
                    </button>
                    <button className={` rounded-md w-1/2 p-3 flex justify-center hover:bg-gray-700 items-center border border-gray-600 bg-gray-800`}>
                        <FaXmark size={18} color='red'/>
                    </button>
                </div>
            </div>
        </div>
    )
  }

const FriendNotification = ({title, message, from, username}) => {
    const store = useStore()
    return (
        <div className="w-full bg-gray-600 border border-1 mb-1 px-3 py-4 text-white border-slate-600">
            <div className="flex text-left flex-col items-center justify-left">
                <h3 className="text-slate-100 font-bold text-lg">{title}</h3>
                <p>{message}</p>
                <div className='w-full flex flex-row justify-around'>
                    <button onClick={() => {
                      store.AcceptFriendRequest(username)
                    }} className={`rounded-md w-1/2 p-3 flex justify-center hover:bg-gray-700 items-center border border-gray-600 bg-gray-800`}>
                        <FaCheck size={18} color='green'/>
                    </button>
                    <button className={` rounded-md w-1/2 p-3 flex justify-center hover:bg-gray-700 items-center border border-gray-600 bg-gray-800`}>
                        <FaXmark size={18} color='red'/>
                    </button>
                </div>
            </div>
        </div>
    )
  }

const Sidebar = () => {
    const [isOpen, setIsOpen] = useState(false)
    const [tab, setTab] = useState('Friends')
    const friendRequestSent = usePopup(false)
    const store = useStore()
    const bgCol = 'bg-gray-800'

    const getTab = () => {
        if(!isOpen) {
            return null
        }
        switch(tab) {
            case 'Friends': return <FriendsList/>
            case 'Notifications': return <Notifications/>
        }
    }
    const handleAddFriend = (event) => {
        if (event.key === 'Enter') {
            store.SendFriendRequest(event.target.value)
            friendRequestSent.open()
            event.target.value = ''
            event.target.blur()
        }
      }
    const selectedCss = 'border border-gray-600 bg-gray-800'
    return (
        <>
            <div className={friendRequestSent.style}>
                <div className="flex flex-col w-1/2 bg-gray-900 border border-gray-800 px-5 py-10 items-center justify-center rounded-sm">
                    <h2 className="text-white text-xl mb-3">Friend Request Sent</h2>
                    <button className={`w-1/2 bg-blue-700 hover:bg-blue-600 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline`} onClick={friendRequestSent.closeOnClick}>Ok</button>
                </div>
            </div>
            <div onClick={() => setIsOpen(!isOpen)} style={{padding: '0.1rem'}} className={`${isOpen ? 'hidden' : 'block'} fixed right-0 ${bgCol} semi top-1/2`}>
                { isOpen ? <FaCaretRight size={20} color='gray'/> : <FaCaretLeft size={20} color='gray'/> }
            </div>
            <div className={`relative ${bgCol} ${isOpen ? 'w-1/4' : 'w-0 hidden'} h-screen`}>
                <div className={`${isOpen ? 'w-full' : 'w-0'} bg-gray-900 p-2 font-bold`}>
                    <h2 className='text-slate-400 text-center'>{tab}</h2>
                </div>
                {getTab()}
                <div onClick={() => setIsOpen(!isOpen)} style={{left: '-20px', padding: '0.1rem'}} className={`absolute ${bgCol} semi top-1/2`}>
                    { isOpen ? <FaCaretRight size={20} color='gray'/> : <FaCaretLeft size={20} color='gray'/> }
                </div>
                <div className={`absolute bottom-0 w-full bg-gray-900 ${isOpen ? '' : 'hidden'}`}>
                    <input onKeyDown={handleAddFriend} className={`${tab === 'Notifications' ? 'hidden' : ''} text-white w-full bg-gray-600 p-2 mb-1`} placeholder='Add Friend...'/>
                    <div className='w-full flex flex-row justify-around'>
                        <button onClick={() => setTab('Friends')} className={`rounded-md w-1/2 p-3 flex justify-center items-center ${tab === 'Friends' ? selectedCss : ''}`}>
                            <FaUsers size={18}/>
                        </button>
                        <button onClick={() => setTab('Notifications')} className={`rounded-md w-1/2 p-3 flex justify-center items-center ${tab === 'Notifications' ? selectedCss : ''}`}>
                            <div className={`${store.notifications.length < 1 ? 'hidden' : ''} relative`}>
                                <div style={{top: '-18px', right: '-28px'}} className='absolute bg-red-400 text-xs font-bold text-slate-100 h-4 w-4 rounded-full flex items-center justify-center'>
                                    <p>{store.notifications.length}</p>
                                </div>
                            </div>
                            <FaBell size={18}/>
                        </button>
                    </div>
                </div>
            </div>
        </>
    )
}

export default observer(Sidebar)