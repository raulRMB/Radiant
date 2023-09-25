import React, { useEffect, useState } from 'react';
import { Login } from './Login'
import Lobby from './Lobby'
import './assets/index.css';
import useStore from './store';
import { observer } from 'mobx-react';
import { Logout } from './Logout';
import { Register } from './Register';
import Sidebar from './Sidebar';

const Patching = observer(() => {
  const store = useStore()
  return (
    <div className='fixed text-slate-300 bottom-0 left-0 m-2 p-3 bg-gray-700 rounded-md text-lg w-60 flex justify-center items-center text-center flex-col text-white'>
      <h3 className="font-bold">Patching...</h3>
      <div className="relative w-full h-10 rounded-md bg-gray-500 text-center">
        <div className="h-10 rounded-md bg-blue-500 absolute" style={{width: store.patchingPercent + '%'}}>
        </div>
        <div className='h-10 absolute w-full flex items-center justify-center'>
          <p className="font-bold text-sm">{store.patchingPercent}%</p>
        </div>
      </div>
    </div>
  )
})

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
      {store.patching ? <Patching/> : null}
      {store.loggedIn ? <Sidebar/> : null}
      {store.loggedIn ? <Logout/> : null}
    </div>
  )
};

export default observer(App)
