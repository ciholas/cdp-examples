const prompt = require('prompt-sync')({sigint: true});
const GROUP = prompt('GROUP: '); // multicast address
const PORT = prompt('PORT: ');
const HOST = prompt('IFACE: ');

const cdpListener = require('./cdp_listener').cdpListener;

const http = require('http');
const server = http.createServer();
server.listen(9898);

const WebSocketServer = require('websocket').server;
const wsServer = new WebSocketServer({
  httpServer: server
})

// triggered when user connects to webpage
wsServer.on('request', function(request) {

  const connection = request.accept(null, request.origin);
  console.log("Client connected");

  cdpListener(PORT, HOST, GROUP, (object) => {
    connection.send(JSON.stringify(jsonify(object)));
  })

  connection.on('close', function(reasonCode, description) {
    console.log("Client disconnected.");
  })
})

// since we use bigint, we have to make our own JSON stringifier
function jsonify(cdpObject){
  return JSON.parse(JSON.stringify(cdpObject, (key, value) =>
    typeof value === 'bigint' ? value.toString() : value
  ));
}
