import React, { useEffect, useState } from 'react';
import { Login } from './Login'
import Lobby from './Lobby'
import './assets/index.css';
import useStore from './store';
import { observer } from 'mobx-react';
import { Logout } from './Logout';
import { Register } from './Register';
import Sidebar from './Sidebar';

const App = () => {

  const store = useStore()

  const [registerOpen, setRegisterOpen] = useState(false)

  const getRoute = () => {
    if(!store.loggedIn) {
      if(registerOpen) {
        return <Register setRegisterOpen={setRegisterOpen}/>
      }
      return <Login setRegisterOpen={setRegisterOpen}/>
    }
    if(store.inMatch) {
      return <p>In Match...</p>
    } else {
      return <Lobby/>
    }
  }

  return (
    <div className='flex justify-between'>
      {getRoute()}
      {store.loggedIn ? <Sidebar/> : null}
      {store.loggedIn ? <Logout/> : null}
    </div>
  )
};

export default observer(App)
