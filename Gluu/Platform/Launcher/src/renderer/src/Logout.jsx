import React, { useEffect, useRef, useState } from 'react';
import useStore from './store';

export const Logout = () => {
    const store = useStore()
    return (
        <div className="fixed bottom-0 left-0 m-4">
            <button onClick={(e) => {
                e.preventDefault()
                store.Logout()
            }} className="border border-slate-600 w-full bg-transparent hover:bg-gray-800 text-slate-600 font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline">
                Logout
            </button>
        </div>
    )
}