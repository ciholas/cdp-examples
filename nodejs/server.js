const PORT = 7667;
const HOST = '0.0.0.0';
const GROUP = '239.255.76.67' // multicast address

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
