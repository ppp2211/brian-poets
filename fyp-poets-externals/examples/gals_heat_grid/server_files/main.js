// creates a node.js based server used to render the incoming firefly data from the POETS engine

const express = require('express');
const app = express();
const WebSocket = require('ws');

// create named pipe for sending messages back to the executive
var fs = require('fs')
const wstream = fs.createWriteStream('./to_poets_devices.sock')

// reading from STDIN: https://stackoverflow.com/questions/20086849/how-to-read-from-stdin-line-by-line-in-node
var readline = require('readline');
var r1 = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false
}); 

var fs = require('fs');
var path = require('path');

// open our web socket
const wss = new WebSocket.Server({port: 8079});

// send flash to the rendered instance
function sendFlash(wss, firefly) {
    // https://github.com/websockets/ws#simple-server
    wss.clients.forEach(function each(client) {
     if(client !== wss && client.readyState == WebSocket.OPEN) {
      client.send(firefly);
     }
    });
}

// receive a message (these will be forwarded down to the POETS devices)
wss.on('connection', function(ws) {
  ws.on('message', function incoming(data) {
    // write to a named pipe that will be picked up by the executive and passed to pts-serve
    wstream.write(data+';');
  })
})

// get stdin data which will be passed to the rendered graph
// this will be the output of the executive
r1.on('line', function(line) {
  sendFlash(wss, line); 
});

app.get('/', (req, res) => res.sendFile(path.join(__dirname+'/index.html')));
app.get('/data.json', (req, res) => res.sendFile(path.join(__dirname+'/data.json')));

app.listen(3000, () => console.log('listening on port 3000'))
  
