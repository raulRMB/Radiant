var socket = io()
document.addEventListener("DOMContentLoaded", () => {
    document.getElementById('addServer').onclick = onAddServer;
    socket.on('reload', () => location.reload())
})

function onAddServer() {
    socket.emit('addServer')
}


function restartServer(name) {
    socket.emit('restartServer', {name: name})
}


function deleteServer(name) {
    socket.emit('removeServer', {name: name})
}