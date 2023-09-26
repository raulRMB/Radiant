import { useState } from 'react'

const usePopup = (init) => {

    const [open, setOpen] = useState(init)

    const closeOnClick = (e) => {
        if(e.target === e.currentTarget) {
            setOpen(!open)
        }
    }

    return {
        status: open,
        toggleOnClick: () => setOpen(!open),
        closeOnClick: closeOnClick,
        style: open ? "fixed left-0 top-0 h-screen w-full text-center flex justify-center items-center z-50 bg-black bg-opacity-25" : "hidden",
        open: () => setOpen(true),
        close: () => setOpen(false)
    }
}

export default usePopup