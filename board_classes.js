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
function xChType (color, status, killed, pos=-1) {
        return {"color":color, "status":status, "killed":killed, "pos":pos }
    }

class xChecker {
    constructor(xCh) {
    var ind = 0
    this.xCh = xCh
    if (this.xCh.color == 'black') { ind += 0 } else { ind += 1 }
      if (this.xCh.status == 'simple') { ind += 0 } else { ind += 2 }
      if (this.xCh.killed == 'no') { ind += 0 } else { ind += 4 }
    var ch = document.createElement('object')
    ch.data = svgFilse[ind]
    ch.type = 'image/svg+xml'
    return ch
  }
}
