import fs from 'fs'
import crypto from 'crypto'

export default (app, __dirname) => { 
  app.get('/patchData/blocks/:id', (req, res) => {
      if(req.params.id) {
        const path = `./data/patchData/blocks/${req.params.id}`
        fs.stat(path, function(err, stat) {
          if (err == null) {
            res.sendFile(path, { root: __dirname })
          } else {
            res.sendStatus(404)
            console.log(`File doesn't exist ${err}`)
          }
        });
      }
  })
  
  app.get('/patchData/dictionary', (req, res) => {
    const path = `./data/patchData/dictionary`
      fs.stat(path, function(err, stat) {
        if (err == null) {
          res.sendFile(path, { root: __dirname })
        } else {
          res.send('not found')
          console.log(`File doesn't exist ${err}`)
        }
      });
  })

  app.get('/patchData/patchData.json', (req, res) => {
      const path = `./data/patchData/patchData.json`
      fs.stat(path, function(err, stat) {
        if (err == null) {
          res.sendFile(path, { root: __dirname })
        } else {
          res.send('bundle not found')
          console.log(`File doesn't exist ${err}`)
        }
      });
  })
    
  app.get('/patch/version', (req, res) => {
      const path = `./data/patchData/patchData.json`
      const fileBuffer = fs.readFileSync(path)
      const hashSum = crypto.createHash('sha256')
      hashSum.update(fileBuffer)
      const hex = hashSum.digest('hex')
      res.json({version: hex})
  })
    
  app.get('/patchData/bundles/:id', (req, res) => {
      if(req.params.id) {
        const path = `./data/patchData/bundles/${req.params.id}`
        fs.stat(path, function(err, stat) {
          if (err == null) {
            res.sendFile(path, { root: __dirname })
          } else {
            res.send('bundle not found')
            console.log(`File doesn't exist ${err}`)
          }
        })
      }
  })
}