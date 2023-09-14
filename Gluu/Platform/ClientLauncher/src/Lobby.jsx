import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import 'animate.css';

export const Lobby = ({ socket }) => {

  const handleSubmit = e => {
    e.preventDefault();
    const formData = new FormData(e.target)
    var lobbyData = {};
    formData.forEach((value, key) => {
        lobbyData[key] = value;
    });
    socket.emit('joinQueue', {queue: '1v1'})
  }

  return (
  <div className="w-full bg-gray-800 h-screen flex items-center justify-center flex-col">
    <h2 className="text-white text-4xl">Lobby</h2>
    <form method="post" className="bg-white shadow-md rounded px-8 pt-6 pb-8 mb-4 w-1/3" onSubmit={handleSubmit}>
      <div className="flex items-center justify-between">
        <button type="submit" className="w-full bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
          Find Match
        </button>
      </div>
    </form>
  </div>
  );
};
