import React, { useEffect, useState } from 'react';
import { Login } from './Login'
import { Lobby } from './Lobby'
import './assets/index.css';
import { socket } from './socket.js';
import sEvents from '../../../../socketEvents.mjs'

export default () => {
  const [appLoaded, setAppLoaded] = useState(false);
  const [socketConnected, setSocketConnected] = useState(false);
  const [loggedIn, setLoggedIn] = useState(false);

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
      setLoggedIn(false)
    };
    socket.on(sEvents.connect, connectHandler);
    socket.on(sEvents.disconnect, disconnectHandler);
    socket.on(sEvents.notify.loginResponse, () => setLoggedIn(true))
    socket.on(sEvents.notify.logout, () => {
      console.log('logout')
      setLoggedIn(false)
    })
    socket.on(sEvents.notify.matchFound, (msg) => {
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
  return (
  loggedIn ? <Lobby socket={socket}/> : <Login socket={socket}/>
  );
};
