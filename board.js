var addEvent = function (object, type, callback) {
  if (object == null || typeof (object) == 'undefined') return
  if (object.addEventListener) {
    object.addEventListener(type, callback, false)
  } else if (object.attachEvent) {
    object.attachEvent('on' + type, callback)
  } else {
    object['on' + type] = callback
  }
}
function min (x, y) { return x < y ? x : y; }
function animateTo (Ch, dx, dy, duration) {
  Board.busy = true
  var start = Date.now(); // сохранить время начала
  var x = parseFloat(Ch.style.left)
  var y = parseFloat(Ch.style.top)
  w = parseFloat(Ch.style.width)

  var timer = setInterval(function () {
    // вычислить сколько времени прошло из opts.duration
    var timePassed = Date.now() - start

    var compl = timePassed / duration
    if (compl > 1) compl = 1
    Ch.style.left = x + dx * compl + '%'
    Ch.style.top = y + dy * compl + '%'
    var scale = ((compl < 0.5) ? 1 + compl / 2 : 1.5 - compl / 2)
    Ch.style.width = w * scale + '%'
    Ch.style.height = w * scale + '%'
    if (timePassed > duration) {
      Board.busy = false
      clearInterval(timer)
    }
  }, 20)
}
function moveFromTo (posFrom, posTo) {
  var Ch_ = {}
  var list = document.getElementsByClassName('ch')
  for (var i = 0; i < list.length; i++)
    if (list[i].pos == posFrom) Ch_ = list[i]
  for (var i = 0; i < list.length; i++) list[i].style.zIndex = 1
  var posFrom = (Board.side == 'white') ? posFrom : Board.size * Board.size - 1 - posFrom
  var posTo = (Board.side == 'white') ? posTo : Board.size * Board.size - 1 - posTo
  var vectX = posTo % Board.size - posFrom % Board.size
  var vectY = (posTo / Board.size >> 0) - (posFrom / Board.size >> 0)
  var trX = vectX * pW
  var trY = -vectY * pW
  Ch_.style.zIndex = 2
  animateTo(Ch_, trX, trY, 700)
}
function xBrdPos (pos) {
  return (Board.side == 'white') ? pos : Board.size * Board.size - 1 - pos
}

function xDravChrs () {
  var myNode = document.getElementsByClassName('ch')
  while (myNode.length > 0) {
    myNode[0].remove()
  }
  for (var i = 0; i < Board.list.length; i++) {
    var element = Board.list[i].element
    element.pos = Board.list[i].pos
    var pos = xBrdPos(element.pos)
    var x = pos % Board.size
    var y = pos / Board.size >> 0
    element.style.position = 'absolute'
    element.style.left = (x + 0.1) * pW + '%'
    element.style.top = (Board.size - 1 - y + 0.1) * pW + '%'
    element.style.width = pW * 0.8 + '%'
    element.style.height = pW * 0.8 + '%'
    element.style.opacity = 1
    wBoardDiv.appendChild(element)
  }
}

function xDrawPoints () {
  var pnt = []
  pnt[0] = new SVG('board')
  pnt[0].circle('100%').attr({cx: '50%',cy: '50%'})
  pnt[0].zIndex = 5
}

window.onload = function () {}
var Board = {}
xDrawPoints()
Board.busy = false
Board.size = 8
Board.id = -1
Board.side = 'white'
Board.list = {}
var wBoardDiv = document.createElement('div')
var brd = document.getElementById('board')
brd.appendChild(wBoardDiv)
for (i = 0; i < Board.size; i++) {
  var vertL = document.createElement('div')
  vertL.textContent = String.fromCharCode('1'.charCodeAt(0) + i)
  vertL.className = 'vert'
  vertL.ind = i
  vertL.side = 'L'
  wBoardDiv.appendChild(vertL)
  var vertR = document.createElement('div')
  vertR.textContent = String.fromCharCode('1'.charCodeAt(0) + i)
  vertR.className = 'vert'
  vertR.ind = i
  vertR.side = 'R'
  wBoardDiv.appendChild(vertR)
  var gorB = document.createElement('div')
  gorB.textContent = String.fromCharCode('A'.charCodeAt(0) + i)
  gorB.className = 'gor'
  gorB.ind = i
  gorB.side = 'B'
  wBoardDiv.appendChild(gorB)
  var gorT = document.createElement('div')
  gorT.textContent = String.fromCharCode('A'.charCodeAt(0) + i)
  gorT.className = 'gor'
  gorT.ind = i
  gorT.side = 'T'
  wBoardDiv.appendChild(gorT)
}
function xDrawBoard () {
  // if (Board.busy) return false
  var sz = min(window.innerHeight * 0.9, window.innerWidth * 0.7)
  brd.style.width = sz + 'px'
  brd.style.height = sz + 'px'
  wBoardDiv.style.position = 'absolute'
  wBoardDiv.style.width = '90%'
  wBoardDiv.style.height = '90%'
  wBoardDiv.style.top = '5%'
  wBoardDiv.style.left = '5%'
  wBoardDiv.style.outline = '2px solid #817162'
  wBoardDiv.style.outlineOffset = '-1px '
  wBoardDiv.style.backgroundColor = '#EADACA'
  sW = wBoardDiv.clientWidth / Board.size

  pW = 100 / Board.size
  var pH = 0.05
  var elems = wBoardDiv.getElementsByClassName('vert')
  for (i = 0; i < elems.length; i++) {
    var st = elems[i].style
    st.position = 'absolute'
    st.width = pW * 0.3 + '%'
    st.height = pW + '%'
    if (elems[i].side == 'L') st.left = -pW * 0.3 + '%'
    if (elems[i].side == 'R') st.left = 100 + '%'
    var j
    if (Board.side != 'white') j = elems[i].ind
    else j = Board.size - elems[i].ind - 1
    st.top = pW * (j) + '%'
    st.fontSize = sW * 0.25 + 'px'
    st.lineHeight = sW + 'px'
    st.textAlign = 'center'
  }
  var elems = wBoardDiv.getElementsByClassName('gor')
  for (i = 0; i < elems.length; i++) {
    var st = elems[i].style
    st.position = 'absolute'
    st.height = pH + '%'
    st.width = pW + '%'
    if (elems[i].side == 'T') st.top = -pW * 0.3 + '%'
    if (elems[i].side == 'B') st.top = 100 + '%'
    var j
    if (Board.side == 'white') j = elems[i].ind
    else j = Board.size - elems[i].ind - 1
    st.left = pW * j + '%'
    st.fontSize = sW * 0.25 + 'px'
    st.textAlign = 'center'
  }
  var elems = wBoardDiv.getElementsByClassName('cell')
  for (i = 0; i < elems.length; i++) {
    var pos = xBrdPos(elems[i].pos)
    var x = pos % Board.size
    var y = pos / Board.size >> 0
    elems[i].style.top = (Board.size - 1 - y) * pW + '%'
    elems[i].style.left = x * pW + '%'
    elems[i].style.width = pW + '%'
    elems[i].style.height = pW + '%'
  }
  xDravChrs()
}

function start () {
  for (i = 0; i < Board.size * Board.size; i++) {
    if (((i / Board.size >> 0) + i % Board.size) % 2 != 0) continue
    var cell = document.createElement('div')
    cell.className = 'cell'
    cell.style.backgroundColor = '#817162'
    cell.style.position = 'absolute'
    cell.pos = i
    // cell.textContent = i
    wBoardDiv.appendChild(cell)
  }
  xDrawBoard()
  websocket = new WebSocket('ws://localhost:1234/')
  initWebSocket()
}
addEvent(window, 'load', start)
addEvent(window, 'resize', xDrawBoard)
