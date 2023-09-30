import { app, shell, BrowserWindow } from 'electron'
import { exec, spawn } from 'child_process'
import { join } from 'path'
import { electronApp, optimizer, is } from '@electron-toolkit/utils'
import icon from '../../resources/icon.png?asset'
import fs from 'fs'
import path from 'path'
import { homedir } from 'os'
import crypto from 'crypto'
import checkElevated from 'native-is-elevated'
const isElevated = checkElevated();

const { ipcMain } = require('electron')
ipcMain.on('matchFound', (event, msg) => {
  exec(`"C:/itch/radiant 2/RadiantClient.exe" ${msg.ip}:${msg.port} -fullscreen`, (error, stdout, stderr) => { 
    console.log(stdout)
  });
});

let mainWindow
const folderName = "/RadiantGames"
let installDirectory
let appDataPath
if(process.platform == 'win32') {
  installDirectory = "C:/Program Files" + folderName + '/install'
  appDataPath = "C:/Program Files" + folderName + '/data'
}
else if(process.platform == 'darwin') {
  installDirectory = '/Applications' + folderName
  appDataPath =  homedir() + '/Library/Application Support' + folderName
}

const getVersionHash = async () => {
  const path = `${appDataPath}/patchData.json`
  try{
    fs.accessSync(path, fs.constants.F_OK);
  }catch(e){
    return {hash: null, isElevated}
  }
  const fileBuffer = fs.readFileSync(path);
  const hashSum = crypto.createHash('sha256');
  hashSum.update(fileBuffer);
  return {hash: hashSum.digest('hex'), isElevated};
}

ipcMain.handle('get-version', async () => {
  const version = getVersionHash();
  const res = await fetch('https://rtb.nyc3.cdn.digitaloceanspaces.com/patchData/version.txt')
  const hash = await res.text()
  return {remote : hash, local : version.hash}
});

function findPatcherScript() {
  const possibilities = [
    // In packaged app
    path.join(process.resourcesPath, "patcher", "patch.py"),
    // In development
    "./patcher/patch.py",
  ];
  for (const path of possibilities) {
    if (fs.existsSync(path)) {
      return path;
    }
  }
  console.log("Could not find python3, checked", possibilities);
  app.quit();
}

ipcMain.on('update', (event, msg) => {
  const patchPath = findPatcherScript()
  const patcher = spawn('python3', ['-u', patchPath]);

  patcher.stdout.on('data', function (data) {
    mainWindow.webContents.send('patching-update', data.toString());
  });

  patcher.stderr.on('data', function (data) {
    console.log('stderr: ' + data.toString());
  });

  patcher.on('exit', function (code) {
    console.log('child process exited with code ' + code.toString());
  });
})

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 900,
    height: 670,
    show: false,
    autoHideMenuBar: true,
    ...(process.platform === 'linux' ? { icon } : {}),
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false
    }
  })

  mainWindow.on('ready-to-show', () => {
    mainWindow.show()
  })

  mainWindow.webContents.setWindowOpenHandler((details) => {
    shell.openExternal(details.url)
    return { action: 'deny' }
  })

  // HMR for renderer base on electron-vite cli.
  // Load the remote URL for development or the local html file for production.
  if (is.dev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  // Set app user model id for windows
  electronApp.setAppUserModelId('com.electron')
  // Default open or close DevTools by F12 in development
  // and ignore CommandOrControl + R in production.
  // see https://github.com/alex8088/electron-toolkit/tree/master/packages/utils
  app.on('browser-window-created', (_, window) => {
    optimizer.watchWindowShortcuts(window)
  })

  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

// In this file you can include the rest of your app"s specific main process
// code. You can also put them in separate files and require them here.
