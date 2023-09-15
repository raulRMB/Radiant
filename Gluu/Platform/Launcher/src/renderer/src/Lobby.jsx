import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import 'animate.css';
import sEvents from '../../../../socketEvents.mjs'

export const Lobby = ({ socket }) => {

  const [inQueue, setInQueue] = useState(false);

  const handleSubmit = e => {
    e.preventDefault();
    if(!inQueue) {
      const formData = new FormData(e.target)
      var lobbyData = {};
      formData.forEach((value, key) => {
          lobbyData[key] = value;
      });
      socket.emit(sEvents.joinQueue, {queue: '1v1'})
    } else {
      socket.emit(sEvents.cancelQueue)
    }
  }

  const joinQueueResponse = (msg) => {
    if(msg.success) {
      setInQueue(true)
    }
  }

  useEffect(() => {
    socket.on(sEvents.notify.joinQueueResponse, joinQueueResponse)
    socket.on(sEvents.notify.cancelQueueResponse, () => setInQueue(false))
    return () => {
      socket.off(sEvents.joinQueueResponse, joinQueueResponse);
    };
  }, []);

  const buttonText = inQueue ? 'Cancel Finding Match...' : 'Find Match' 
  return (
  <div className="w-full bg-gray-800 h-screen flex items-center justify-center flex-col">
    <h2 className="text-white text-4xl">Lobby</h2>
    <form method="post" className="bg-white shadow-md rounded px-8 pt-6 pb-8 mb-4 w-1/3" onSubmit={handleSubmit}>
      <div className="flex items-center justify-between">
        <button type="submit" className="w-full bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
        {buttonText}
        </button>
      </div>
    </form>
  </div>
  );
};
