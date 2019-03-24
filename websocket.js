var websocket = null
function SocketCall (fName, fArgs) {
  var msg = { 'Call': fName, 'args': fArgs }
  websocket.send(JSON.stringify(msg))
}
function CallAndRuturnToSocket (xS) {
  var fn = window[xS.funct]
  if (typeof fn === 'function') {
    res = fn(xS.args)
    websocket.send(JSON.stringify({ 'funct': xS.funct, 'result': res }))
  }
}

function GetId (args) {
  return wBoard.id
}
function SetId (args) {
  wBoard.id = args.id
}

function initWebSocket () {
  websocket = new WebSocket('ws://localhost:1234')
  websocket.onopen = function () {
    document.getElementById('b1').textContent = 'Connected!'
  }
  websocket.onmessage = function (ev) {
    console.log(ev)
    xS = JSON.parse(ev.data)
    if (xS.cmd == 'Call') CallAndRuturnToSocket(xS)
  }
}

function sendMessage () {
  websocket.send(document.getElementById('messageInput').value)
}
