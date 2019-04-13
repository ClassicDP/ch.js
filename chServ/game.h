#ifndef GAME_H
#define GAME_H
#include "inttypes.h"
typedef uint8_t  u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#include "itemlist.h"
#include <stdint.h>
#include <qstring.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QObject>


enum checker_type { _simple=0, _king=1};
enum checker_color {_black=0, _white=1};
enum checker_killed {_yes=1,_no=0};

class CheckerXY
{
public:
    uint PtrCnt=0;//the number of pointers to this object
    uint8_t x,y;
    checker_killed killed=_no;
    checker_type type;
    checker_color color;
    CheckerXY (checker_color color, checker_type type, uint8_t x, uint8_t y);
    inline bool operator ==(const CheckerXY &ch);
};
struct BaseItem
{
    uint PtrCnt=0;//the number of pointers to this object
};

struct Ch:BaseItem
{
    int8_t pos;// pos - position on board
    checker_type type;
    checker_color color;
    Ch(int8_t pos, checker_type type, checker_color color):pos(pos),type(type),color(color){}
    Ch(){}
};

class MoveItem
{
public:
    uint PtrCnt=0;//the number of pointers to this object
    MoveItem * conn;//chain connection
    int8_t from, to;
    int8_t kill;
    MoveItem(MoveItem * conn, int8_t from, int8_t to, int8_t kill=-1);
    ~MoveItem();
    inline bool operator ==(const MoveItem &move);
    QJsonObject toJson()
    {
        return QJsonObject ({{"from",from},{"to", to}, {"kill",kill}});
    }
};

struct MoveTreeItem
{
    uint8_t PtrCnt=0;
    MoveItem * move;
    ItemsList <MoveTreeItem> * next, *prev;
    MoveTreeItem(MoveItem * move);
    QString toString(uint8_t Size=8);

};


template <typename T>
void fill0 (T* first, int n)
{
    while (n>0) {
        *first = nullptr;
        ++first; --n;
    }
}


class Position
{
public:
    uint PtrCnt=0;//the number of pointers to this object
    ItemOfList <Ch> ** board;//checkers on board; pointers in checkers list
    int8_t Size;
    int len;//len of board array
    checker_color ClrOfMove;//color of next move
    ItemsList <Ch> *ChList[2];//checkers on board
    MoveItem * move;//Move track (chain to back);
    QJsonObject toJson();
    Position (int8_t Size, ItemsList <CheckerXY> * CheckersList, checker_color nextMoveColor);
    Position (int8_t Size, checker_color nextMoveColor);
    Position (Position * pos);
    QString PosToStr(int8_t pos);
    QString MoveAsStr();

    ~Position();
};

inline checker_color reverse(checker_color x)
{
    return (x==_white)?_black:_white;
}
inline bool is_king(Position *ps, int8_t x)
{
    return  ( ( (ps->ClrOfMove==_black) && (x<ps->Size) ) ||
              ( (ps->ClrOfMove==_white) && (x>=(ps->len-ps->Size)) ));
}


ItemsList <MoveTreeItem> *PositionListToMoveTree(ItemsList <Position> * pList);
class GameFunctions
{
    int8_t ve[4];//vector of directoins
    int8_t Size;
    int len;
    inline bool isOnBoard(int8_t pos);
public:
    GameFunctions(int8_t Size);

    bool TryToKill(Position *ps, ItemsList<Position> *PosList,
                   MoveItem *move=NULL, int8_t skipDir=-1, bool WasMoreDeep=false);
    bool TryToMove (Position * ps, ItemsList <Position> * PosList, MoveItem * move=NULL);
    bool MakeMovesList(Position * ps, ItemsList <Position> * PosList);

};

class PTreeMoves
{
public:
    ItemsList <MoveTreeItem> * top=new ItemsList <MoveTreeItem>, *saveTop=nullptr;
    ItemsList <MoveTreeItem> * track=new ItemsList <MoveTreeItem>;
    ItemsList <MoveTreeItem> * Variants=new ItemsList <MoveTreeItem>;
    ItemsList <MoveItem> * MoveList=new ItemsList <MoveItem>;
    ItemsList <Ch> * to_delete= new ItemsList <Ch>;
    bool Wait_for_First=true;
    ItemsList <MoveTreeItem> * MoveVariants(int8_t pos);

    void DeleteTrack();
    ~PTreeMoves();

    PTreeMoves(ItemsList<Position> *pList);
};


class Game: public QObject
{
Q_OBJECT
public:
    Position * Pos;
    ItemsList <Ch> * last_points=new ItemsList <Ch>;
    int8_t size=8;
    GameFunctions routs;
    Game(int8_t size);
    ~Game() {delete Pos; delete  last_points;}
    void StartPos ();
    void TrackLoop(int8_t from_pos);
    QJsonObject CallFunct(QString functName);
    QJsonObject CallFunct(QString functName, QJsonObject args);
signals:
    QJsonObject  Call(QString functName, QJsonObject args);
public slots:

};


#endif // GAME_H
