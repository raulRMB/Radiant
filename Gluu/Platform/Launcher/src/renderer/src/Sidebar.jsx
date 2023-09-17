import React, { useEffect, useRef, useState } from 'react';
import { FaCaretLeft, FaCaretRight, FaUsers, FaBell, FaCheck } from 'react-icons/fa';
import { FaXmark } from 'react-icons/fa6'
import sEvents from '../../../../socketEvents.mjs'

const FriendCard = ({username, status}) => {
    const getColorFromStatus = () => {
        switch(status) {
            case 'Online': return 'bg-green-200'
            case 'Offline': return 'bg-gray-300'
            case 'Away': return 'bg-red-400'
            case 'In Game': return 'bg-blue-400'
        }
    }
    return (
        <div className="w-full bg-gray-600 border border-1 px-3 mb-1 py-4 text-white hover:bg-gray-700 border-slate-600">
            <div className="flex flex-row items-center justify-left">
                <div className={`rounded-full ${getColorFromStatus()} w-3 h-3 mx-1`}></div>
                <h3>{username}</h3>
            </div>
        </div>
    )
}

const FriendsList = () => {
    return (
        <div className='w-full'>
            {/* <FriendCard username="SuperMassive" status="Online"/>
            <FriendCard username="Mashcroft" status="In Game"/>
            <FriendCard username="Nelii" status="Away"/>
            <FriendCard username="Arlix" status="Offline"/>
            <FriendCard username="Chewy" status="Offline"/>
            <FriendCard username="Mert" status="Offline"/> */}
        </div>
    )
}

const Notifications = ({notifications}) => {
    return (
        <div className='w-full'>
            {notifications}
        </div>
    )
}

export const Sidebar = ({socket, notifications}) => {
    const [isOpen, setIsOpen] = useState(false)
    const [tab, setTab] = useState('Friends')

    const bgCol = 'bg-gray-800'

    const getTab = () => {
        if(!isOpen) {
            return null
        }
        switch(tab) {
            case 'Friends': return <FriendsList/>
            case 'Notifications': return <Notifications notifications={notifications} socket={socket}/>
        }
    }
    const handleAddFriend = (event) => {
        if (event.key === 'Enter') {
            socket.emit(sEvents.addFriend, {username: event.target.value})
        }
      }
    const selectedCss = 'border border-gray-600 bg-gray-800'
    return (
        <>
            <div className={`fixed top-0 right-0 ${bgCol} ${isOpen ? 'w-60' : 'w-0'} h-screen`}>
                <div className="w-full bg-gray-900 p-2 font-bold">
                    <h2 className='text-slate-400 text-center'>{tab}</h2>
                </div>
                {getTab()}
                <div onClick={() => setIsOpen(!isOpen)} style={{left: '-20px', padding: '0.1rem'}} className={`absolute ${bgCol} semi top-1/2`}>
                    { isOpen ? <FaCaretRight size={20} color='gray'/> : <FaCaretLeft size={20} color='gray'/> }
                </div>
                <div className={`absolute bottom-0 w-full bg-gray-900 ${isOpen ? '' : 'hidden'}`}>
                    <input onKeyDown={handleAddFriend} className='text-white w-full bg-gray-600 p-2 mb-1' placeholder='Add Friend...'/>
                    <div className='w-full flex flex-row justify-around'>
                        <button onClick={() => setTab('Friends')} className={`rounded-md w-1/2 p-3 flex justify-center items-center ${tab === 'Friends' ? selectedCss : ''}`}>
                            <FaUsers size={18}/>
                        </button>
                        <button onClick={() => setTab('Notifications')} className={` rounded-md w-1/2 p-3 flex justify-center items-center ${tab === 'Notifications' ? selectedCss : ''}`}>
                            <FaBell size={18}/>
                        </button>
                    </div>
                </div>
            </div>
        </>
    )
}