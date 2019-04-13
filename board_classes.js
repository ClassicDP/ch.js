var svgFilse = [
  './res/черная шашка.svg',
  './res/белая шашка.svg',
  './res/черная дамка.svg',
  './res/белая дамка.svg',
  './res/черная шашка битая.svg',
  './res/белая шашка битая.svg',
  './res/черная дамка битая.svg',
  './res/белая дамка битая.svg'

]
function xChType (color, type, killed, pos = -1) {
  return { 'color': color, 'type': type, 'killed': killed, 'pos': pos }
}

class xChecker {
  constructor (xCh) {
    var img = document.createElement('img')
    this.element = img
    Object.assign(this, xCh)
    this.updImg()
    addEvent(this, 'onclick', function click () {})
  }
  updImg () {
    var ind = 0
    if (this.color == 'black') { ind += 0 } else { ind += 1 }
    if (this.type == 'simple') { ind += 0 } else { ind += 2 }
    if (this.killed == 'no') { ind += 0 } else { ind += 4 }
    this.element.src = svgFilse[ind]
    this.element.type = 'image/svg+xml'
    this.element.className = 'ch'
  }

}

class xChList {
  constructor (js) {
    var list = []
    js.ch.forEach(function (ch) {
      ch.killed = 'no'
      ch = new xChecker(ch)
      list.push(ch)
    })
    this._list = list
  }
  get list () {
    return this._list
  }
  chAt (pos) {
    for (var i in this._list)
      if (this._list[i].pos == pos) return this._list[i]
    return undefined
  }
  set list (list) {
    this._list = list
  }
}
class xMarkDot {
  constructor () {}
}
