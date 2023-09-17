import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import 'animate.css';

export const Register = ({ socket, setRegisterOpen }) => {

  const handleSubmit = e => {
    e.preventDefault();
    const formData = new FormData(e.target)
    var loginData = {};
    formData.forEach((value, key) => {
      loginData[key] = value;
    });
    socket.emit('login', loginData)
  }

  return (
  <div className="w-full bg-gray-800 h-screen flex items-center justify-center flex-col">
    <div className='w-1/3 flex items-center justify-center flex-col'>
    <h2 className="text-white text-6xl p-4">Register Account</h2>
    <form method="post" className="w-full bg-white shadow-md rounded px-8 pt-6 pb-4 mb-4" onSubmit={handleSubmit}>
      <div className="mb-4">
        <label className="block text-gray-700 text-sm font-bold mb-2" htmlFor="username">
          Username
        </label>
        <input name="username" className="shadow appearance-none border rounded w-full py-2 px-3 text-gray-700 leading-tight focus:outline-none focus:shadow-outline" id="username" type="text" placeholder="Username"/>
      </div>
      <div className="mb-6">
        <label className="block text-gray-700 text-sm font-bold mb-2" htmlFor="password">
          Password
        </label>
        <input name="password" className="shadow appearance-none rounded w-full py-2 px-3 text-gray-700 mb-3 leading-tight focus:outline-none focus:shadow-outline" id="password" type="password" placeholder="******************"/>
      </div>
      <div className="flex items-center justify-between">
        <button type="submit" className="w-full bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
          Register
        </button>
      </div>
    </form>
    <p className='text-right text-blue-500 w-full hover:text-blue-300 hover:cursor-pointer' onClick={() => setRegisterOpen(false)}>Back</p>
    </div>
  </div>
  );
};
