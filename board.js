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
function xDrawBoard () {
  var ht = window.innerHeight * 0.9
  var sz = min(ht, window.innerWidth * 0.7)
  brd.style.width = sz + 'px'
  brd.style.height = sz + 'px'

  wBoard.style.position = 'absolute'
  wBoard.style.width = '90%'
  wBoard.style.height = '90%'
  wBoard.style.top = '5%'
  wBoard.style.left = '5%'
  wBoard.style.outline = '2px solid #817162'
  wBoard.style.backgroundColor = '#EADACA'
  sW = wBoard.clientWidth / bParams.size
  sH = wBoard.clientWidth * 0.05
  var elems = wBoard.getElementsByClassName('vert')
  for (i = 0; i < elems.length; i++) {
    var st = elems[i].style
    st.position = 'absolute'
    st.width = sH + 'px'
    st.height = sW + 'px'
    if (elems[i].side == 'L') st.left = -sH + 'px'
    if (elems[i].side == 'R') st.left = wBoard.clientWidth + 'px'
    var j
    if (bParams.side != 'white') j = elems[i].ind
    else j = bParams.size - elems[i].ind - 1
    st.top = sW * j + 'px'
    st.lineHeight = st.height
    st.textAlign = 'center'
  }
  var elems = wBoard.getElementsByClassName('gor')
  for (i = 0; i < elems.length; i++) {
    var st = elems[i].style
    st.position = 'absolute'
    st.height = sH + 'px'
    st.width = sW + 'px'
    if (elems[i].side == 'T') st.top = -sH + 'px'
    if (elems[i].side == 'B') st.top = wBoard.clientWidth + 'px'
    var j
    if (bParams.side == 'white') j = elems[i].ind
    else j = bParams.size - elems[i].ind - 1
    st.left = sW * j + 'px'
    st.lineHeight = st.height
    st.textAlign = 'center'
  }
var elems = wBoard.getElementsByClassName('cell')
for (i = 0;i < elems.length;i++) {
  var pos = (bParams.side == 'white') ? elems[i].pos : bParams.size * bParams.size - 1 - elems[i].pos
  var x=pos%bParams.size
  var y=pos/bParams.size>>0
  elems[i].style.top=(bParams.size-1- y)*sW+'px'
  elems[i].style.left = x * sW + 'px'
  elems[i].style.width = sW + 'px'
  elems[i].style.height = sW + 'px'
}
}

var bParams = {}
bParams.size = 8
bParams.side = 'black'
var wBoard = document.createElement('div')
var brd = document.getElementById('board')
brd.appendChild(wBoard)
for (i = 0; i < bParams.size; i++) {
  var vertL = document.createElement('div')
  vertL.textContent = String.fromCharCode('1'.charCodeAt(0) + i)
  vertL.className = 'vert'
  vertL.ind = i
  vertL.side = 'L'
  wBoard.appendChild(vertL)
  var vertR = document.createElement('div')
  vertR.textContent = String.fromCharCode('1'.charCodeAt(0) + i)
  vertR.className = 'vert'
  vertR.ind = i
  vertR.side = 'R'
  wBoard.appendChild(vertR)
  var gorB = document.createElement('div')
  gorB.textContent = String.fromCharCode('A'.charCodeAt(0) + i)
  gorB.className = 'gor'
  gorB.ind = i
  gorB.side = 'B'
  wBoard.appendChild(gorB)
  var gorT = document.createElement('div')
  gorT.textContent = String.fromCharCode('A'.charCodeAt(0) + i)
  gorT.className = 'gor'
  gorT.ind = i
  gorT.side = 'T'
  wBoard.appendChild(gorT)
}


for (i = 0; i < bParams.size * bParams.size; i++) {
  if (( (i / bParams.size >> 0) + i % bParams.size) % 2 != 0) continue
  var cell = document.createElement('div')
  cell.className = 'cell'
  cell.style.backgroundColor = '#817162'
  cell.style.position = 'absolute'
  cell.pos = i
  cell.textContent=i
  wBoard.appendChild(cell)
}


var ch1 = new xChecker(xChType('black', 'simple', 'no'))
ch1.style.width = '100px'
ch1.style.left = '200px'
ch1.style.top = '200px'
ch1.style.position = 'absolute'
brd.appendChild(ch1)
var ch1 = new xChecker(xChType('wite', 'simple', 'no'))
ch1.style.width = '100px'
ch1.style.left = '250px'
ch1.style.top = '200px'
ch1.style.position = 'absolute'
brd.appendChild(ch1)
addEvent(window, 'load', xDrawBoard)
addEvent(window, 'resize', xDrawBoard)
