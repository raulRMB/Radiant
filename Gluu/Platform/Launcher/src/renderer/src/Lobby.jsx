import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import { FaCrown } from 'react-icons/fa6'
import { observer } from 'mobx-react';
import 'animate.css';
import useStore from './store';

const PlayerCard = ({leader, displayName, isSelf}) => {
  return (
    <div className='w-1/4 shrink-0 px-10 py-20 border border-slate-800 bg-gray-900 hover:border-slate-700 rounded-md flex flex-col items-center justify-center'>
      <div className={`bg-slate-700 ${!isSelf ? 'border border-slate-600' : 'border border-slate-400'} m-2 rounded-full h-20 w-20 flex items-center justify-center`}>
        <p className='font-bold text-white text-3xl'>{displayName[0].toUpperCase()}</p>
      </div>
      <div className='flex items-center justify-center gap-1'>
      {leader && <FaCrown size={18}/>}
      <p className='font-bold text-2xl'>{displayName}</p>
      </div>
      
    </div>
  )
}

const Lobby = () => {
  const store = useStore()
  const handleSubmit = e => {
    e.preventDefault();
    if(!store.inQueue) {
      const formData = new FormData(e.target)
      var lobbyData = {};
      formData.forEach((value, key) => {
          lobbyData[key] = value;
      });
      store.JoinQueue(lobbyData)
    } else {
      store.CancelQueue()
    }
  }

  const players = []
  if(store.lobby && store.lobby.players) {
    store.lobby.players.forEach(player => {
      const isLeader = player.username === store.lobby.leader
      const isSelf = player.username.toUpperCase() === store.username.toUpperCase()
      players.push(<PlayerCard displayName={player.displayName} username={player.username} isSelf={isSelf} leader={isLeader}/>)
    })
  }

  const leader = store.isLeader()
  const buttonText = store.inQueue ? 'Cancel Finding Match...' : leader ? 'Find Match' : 'Waiting For Leader'
  return (
  <div className="w-full bg-black h-screen flex items-center justify-center flex-col">
    <div className="w-full flex gap-10 p-6 items-center justify-center">
      {players}
    </div>
    <form method="post" className="bg-gray-700 shadow-md rounded p-4 mb-4 w-1/3" onSubmit={handleSubmit}>
      <div className="flex items-center justify-between flex-col gap-2">
      <h3 className='text-white w-full text-left font-bold'>Queue:</h3>
      <select name="queue" disabled={store.inQueue || !leader} className="block appearance-none w-full bg-gray-800 border border-gray-500 text-white py-3 px-4 pr-8 rounded leading-tight focus:outline-none focus:bg-gray-800 focus:border-gray-500" id="grid-state">
          <option value="1v1">1v1</option>
          <option value="2v2">2v2</option>
          <option value="3v3">3v3</option>
        </select>
        <button disabled={!leader} type="submit" className={`w-full ${leader ? 'bg-blue-700 hover:bg-blue-600' : 'bg-gray-600'} text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline`}>
          {buttonText}
        </button>
      </div>
    </form>
  </div>
  );
};

export default observer(Lobby)