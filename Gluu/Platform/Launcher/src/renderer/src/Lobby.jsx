import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import { observer } from 'mobx-react';
import 'animate.css';
import useStore from './store';

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

  const buttonText = store.inQueue ? 'Cancel Finding Match...' : 'Find Match' 
  return (
  <div className="w-full bg-black h-screen flex items-center justify-center flex-col">
    <form method="post" className="bg-gray-700 shadow-md rounded px-8 pt-6 pb-8 mb-4 w-1/3" onSubmit={handleSubmit}>
      <div className="flex items-center justify-between flex-col gap-2">
      <h3 className='text-white w-full text-left font-bold'>Select Queue:</h3>
      <select name="queue" disabled={store.inQueue} className="block appearance-none w-full bg-gray-800 border border-gray-500 text-white py-3 px-4 pr-8 rounded leading-tight focus:outline-none focus:bg-gray-800 focus:border-gray-500" id="grid-state">
          <option value="1v1">1v1</option>
          <option value="2v2">2v2</option>
          <option value="3v3">3v3</option>
        </select>
        <button type="submit" className="w-full bg-blue-700 hover:bg-blue-600 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
          {buttonText}
        </button>
      </div>
    </form>
  </div>
  );
};

export default observer(Lobby)