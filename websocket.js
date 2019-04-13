/*
Вызов функции
{
    "cmd": "Call",
    "funct": "GetId"
}

или
{
 "args": {
        "id": 14,
        }
    },
    "cmd": "Call",
    "funct": "SetId"
}
Команды на сервер
{
    "cmd": "loop",
}

формат ответа
 {result: Object, complete: "yes", funct: "GetId"}
 "result": {
            "id": -1
        }

*/

function SocketCall (fName, fArgs) {
  var msg = { 'Call': fName, 'args': fArgs }
  SendMsg(JSON.stringify(msg))
}
function SocketCommand (cmd) {
  var msg = { 'cmd': cmd }
  SendMsg(JSON.stringify(msg))
}
function CallAndRuturnToSocket (xS) {
  var fn = window[xS.funct]
  var res = {}
  if (typeof fn === 'function') {
    res.result = fn(xS.args)
  } else res.error = 'function name error'
  res.complete = 'yes'
  res.funct = xS.funct
  SendMsg(JSON.stringify(res))
}

function GetId (args) {
  return { 'id': Board.id }
}
function SetId (args) {
  Board.id = args.id
}
function SendMsg (_msg) {
  var att = 0
  function sWait () {
    att += 1; //console.log('wait for socket')
  }
  while (websocket.readyState != 1 && att < 3) {
    setTimeout(sWait, 1000)}
  // console.log(att, JSON.parse(_msg))
  if (att < 3) websocket.send(_msg)
}
function SetChOnBoard (list) {
  var lst = new xChList(list)
  Board.list = lst.list
  xDravChrs()
  moveFromTo(0, 9)
}
function ShowPoints (list) {
  Board.points = list
  xShowPoints()
}
function GreenPoints (p) {
 // console.log(p)
}
function initWebSocket () {
  document.getElementById('b1').textContent = 'Connected!'
  for (var ii = 0; ii < 1; ii++) {
    websocket.ii = ii
    websocket.onerror = function (event) {
      //console.log(this.ii, event)
      this.close()
    }
    websocket.onopen = function () {}
    websocket.onmessage = function (ev) {
      xS = JSON.parse(ev.data)

      if (xS.cmd == 'Call')
        CallAndRuturnToSocket(xS)
      if (xS.funct == 'SetChOnBoard')
        setTimeout(function () { SocketCommand('loop')}, 1000)
    }
  }
}

function sendMessage () {
  if (websocket.readyState == 1)
    SendMsg(document.getElementById('messageInput').value)
  else {} //console.log('WS state', websocket.readyState)
}
