import React, { useEffect, useState } from 'react';
import { Login } from './Login'
import { Lobby } from './Lobby'
import './App.css';
import { socket } from './socket.js';

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
    };
    socket.on('connect', connectHandler);
    socket.on('disconnect', disconnectHandler);
    socket.on('loggedIn', () => setLoggedIn(true))
    return () => {
      socket.off('connect', connectHandler);
      socket.off('disconnect', disconnectHandler);
    };
  }, [appLoaded]);

  useEffect(() => {
    console.log('SOCKET CONNECTED', socketConnected);
  }, [socketConnected]);
  return (
  loggedIn ? <Lobby socket={socket}/> : <Login socket={socket}/>
  );
};
