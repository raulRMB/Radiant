import React, { useEffect, useRef, useState } from 'react';
import { FaPlug, FaHandshake } from 'react-icons/fa';
import useStore from './store';
import 'animate.css';

export const Login = ({ setRegisterOpen }) => {
  const store = useStore()
  const handleSubmit = e => {
    e.preventDefault();
    const formData = new FormData(e.target)
    var loginData = {};
    formData.forEach((value, key) => {
      loginData[key] = value;
    });
    store.Login(loginData)
  }

  return (
  <div className="w-full bg-black h-screen flex items-center justify-center flex-col">
    <div className='w-1/3 flex items-center justify-center flex-col'>
    <h2 className="text-white text-6xl p-4">Radiant</h2>
    <form method="post" className="w-full bg-gray-700 text-white shadow-md rounded px-8 pt-6 pb-4 mb-4" onSubmit={handleSubmit}>
      <div className="mb-4">
        <label className="block text-white text-sm font-bold mb-2" htmlFor="username">
          Username
        </label>
        <input name="username" className="shadow appearance-none rounded bg-gray-800 w-full py-2 px-3 text-white leading-tight focus:outline-none focus:shadow-outline" id="username" type="text" placeholder="Username"/>
      </div>
      <div className="mb-6">
        <label className="block text-white text-sm font-bold mb-2" htmlFor="password">
          Password
        </label>
        <input name="password" className="shadow appearance-none rounded bg-gray-800 w-full py-2 px-3 text-white mb-3 leading-tight focus:outline-none focus:shadow-outline" id="password" type="password" placeholder="******************"/>
      </div>
      <div className="flex items-center justify-between">
        <button type="submit" className="w-full bg-blue-700 hover:bg-blue-600 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
          Login
        </button>
      </div>
    </form>
    <div className='w-full'>
      <p className='text-right text-blue-500 hover:text-blue-300 hover:cursor-pointer' onClick={() => setRegisterOpen(true)}>Register Account?</p>
    </div>
    </div>
  </div>
  );
};
