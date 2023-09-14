import io from 'socket.io-client';

let socket;
if (import.meta.env.VITE_ENV === 'production') {
  socket = io(`https://${window.location.host}`, { transports: ['websocket'] });
} else {
  socket = io(`http://localhost:3000`, { transports: ['websocket'] });
}
export { socket };
