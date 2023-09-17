import React, { useEffect, useState } from 'react';
import { Login } from './Login'
import { Lobby } from './Lobby'
import './assets/index.css';
import { socket } from './socket.js';
import sEvents from '../../../../socketEvents.mjs'
import { Logout } from './Logout';
import { Register } from './Register';
import { Sidebar } from './Sidebar';
import { FaCheck } from 'react-icons/fa';
import { FaXmark } from 'react-icons/fa6'

export default () => {
  const [appLoaded, setAppLoaded] = useState(false);
  const [socketConnected, setSocketConnected] = useState(false);
  const [inMatch, setInMatch] = useState(false)
  const [loggedIn, setLoggedIn] = useState(false);
  const [registerOpen, setRegisterOpen] = useState(false)

  const getRoute = () => {
    if(!loggedIn) {
      if(registerOpen) {
        return <Register socket={socket} setRegisterOpen={setRegisterOpen}/>
      }
      return <Login socket={socket} setRegisterOpen={setRegisterOpen}/>
    }
    if(inMatch) {
      return <p>In Match...</p>
    } else {
      return <Lobby socket={socket}/>
    }
  }

  const [notifications, setNotifications] = useState([])

  useEffect(() => {
    setAppLoaded(true);
  }, []);

  useEffect(() => {
    if (!appLoaded) {
      return () => undefined;
    }
    const connectHandler = () => {
      setSocketConnected(true);
    };
    const disconnectHandler = () => {
      setSocketConnected(false);
      console.log('disconnected')
      setNotifications([])
      setLoggedIn(false)
    };
    socket.on(sEvents.connect, connectHandler);
    socket.on(sEvents.disconnect, disconnectHandler);
    socket.on(sEvents.notify.loginResponse, () => setLoggedIn(true))
    socket.on(sEvents.notify.logout, () => {
      setNotifications([])
      console.log('logout')
      setLoggedIn(false)
    })
    socket.on(sEvents.notify.friendRequestReceived, (msg) => {
        console.log(msg)
        setNotifications(notifications => {
          const copy = [...notifications]
          copy.push(<Notification title="Friend Request" message={`${msg.from} sent you a friend request.`} socket={socket} username={msg.from}/>)
          return copy
        });
    })
    socket.on(sEvents.notify.matchFound, (msg) => {
      setInMatch(true)
      window.electron.ipcRenderer.send('matchFound', msg);
    })
    return () => {
      socket.off(sEvents.connect, connectHandler);
      socket.off(sEvents.disconnect, disconnectHandler);
    };
  }, [appLoaded]);

  useEffect(() => {
    console.log('SOCKET CONNECTED', socketConnected);
  }, [socketConnected]);

  const onLogout = () => {
    socket.emit(sEvents.logout)
  }

  return (
    <>
      {getRoute()}
      {loggedIn ? <Sidebar socket={socket} notifications={notifications}/> : null}
      {loggedIn ? <Logout onClick={onLogout}/> : null}
    </>
  )
};

const Notification = ({title, message, socket, username}) => {
  return (
      <div className="w-full bg-gray-600 border border-1 mb-1 px-3 py-4 text-white border-slate-600">
          <div className="flex text-left flex-col items-center justify-left">
              <h3 className="text-slate-100 font-bold text-lg">{title}</h3>
              <p>{message}</p>
              <div className='w-full flex flex-row justify-around'>
                  <button onClick={() => {
                    socket.emit(sEvents.acceptFriendRequest, {username})
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