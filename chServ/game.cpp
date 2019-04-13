#include "game.h"
#include "socketfunct.h"


//not optimised in king steps after killing
bool GameFunctions::isOnBoard(int8_t pos)
{
    len=Size*Size;
    if ((pos<0) || (pos>=Size*Size)) return false;
    return !((pos/Size+pos%Size)%2);
}

GameFunctions::GameFunctions(int8_t Size):Size(Size)
{
    ve[0]=Size-1;
    ve[1]=Size+1;
    ve[2]=-ve[0];
    ve[3]=-ve[1];
}

bool GameFunctions::TryToKill(Position *ps, ItemsList<Position> *PosList, MoveItem *move, int8_t skipDir, bool WasMoreDeep)
{
    bool res=false;
    auto chIt=ps->ChList[ps->ClrOfMove]->CurrentItem();

    for (int8_t i=0; i<4; i++)
    {
        if (i==skipDir) continue;
        int8_t type=chIt->type;
        int8_t x0=chIt->pos; int8_t x1=x0+ve[i];
        //search first not empty field by ve[] direction if _king type
        if (type==_king) while (isOnBoard(x1) && !ps->board[x1]) x1+=ve[i];
        if (!isOnBoard(x1) || !ps->board[x1]) continue;
        if (ps->board[x0]->It->color==ps->board[x1]->It->color) continue;
        int8_t x2=x1;
        //loop if _king type for each empty field after kill-move by ve[] direction
        int8_t SkipDirection=-1;
        auto saveCaseClean_s=PosList->Last;//if false position will be added
        bool MoreDeepWasPossible=false;
        do {
            x2+=ve[i];
            if (!isOnBoard(x2) || ps->board[x2]) break;
            if (!ps->board[x2]) {
                //kill posible!
                res=true;
                auto type= chIt->type;//save status before check for revers
                //check for simple revers to king by move
                if (type!=_king && is_king(ps,x2)) ps->board[x0]->It->type=_king;
                MoveItem * newMove = new MoveItem(move,x0,x2,x1);
                //reverse color of killed checker before recursion//swap for jamp-move
                ps->board[x1]->It->color=reverse(ps->board[x1]->It->color);
                swap_(ps->board[x0],ps->board[x2]);
                chIt->pos=x2;
                auto SaveMoreDeepStatus=MoreDeepWasPossible;auto saveCaseClean_e=PosList->Last;
                MoreDeepWasPossible|=TryToKill(ps, PosList, newMove, SkipDirection, MoreDeepWasPossible);
                //clean false positions from list if more deep move was faund
                if (MoreDeepWasPossible!=SaveMoreDeepStatus)
                {
                    while (saveCaseClean_s!=saveCaseClean_e) {
                        auto tmp=saveCaseClean_e->Prev;
                        PosList->Delete(saveCaseClean_e);
                        saveCaseClean_e=tmp;
                    }
                }
                SkipDirection=i;
                //jamp-move back after return form recursion and set old  checkers type statuses
                ps->board[x1]->It->color=reverse(ps->board[x1]->It->color);
                swap_(ps->board[x0],ps->board[x2]);
                chIt->type=type;
                chIt->pos=x0;
            }
        } while (type==_king);
    }
    if (!res && move && !WasMoreDeep)
    {
        auto newPos=new Position(ps);
        //deleting killed checkers from board and from chList
        newPos->move=move;
        auto xx=move;
        while (xx)
        {
            newPos->ChList[!newPos->ClrOfMove]->Delete(newPos->board[xx->kill]);
            newPos->board[xx->kill]=NULL;
            xx=xx->conn;
        }
        //Add to list
        PosList->AddItem(newPos);
    }
    return res;
}

bool GameFunctions::TryToMove(Position *ps, ItemsList<Position> *PosList, MoveItem *move)
{
    bool res=false;
    auto ChIt=ps->ChList[ps->ClrOfMove]->CurrentItem();
    int8_t a,b;
    //select possible directions
    if (ChIt->type==_king) {a=0;b=4;}
    else if (ChIt->color==_white) {a=0;b=2;} else {a=2;b=4;}
    int8_t x0= ChIt->pos;
    int8_t x1;
    auto type= ChIt->type;//save status for posible revers
    for (int8_t i=a; i<b; i++)
    {
        x1=x0;
        do
        {
            x1+=ve[i];
            if (!isOnBoard(x1) || ps->board[x1]) break;
            //move possible!
            res=true;
            //check for simple revers to king by move
            if (type!=_king && is_king(ps,x1)) ChIt->type=_king;
            ChIt->pos=x1;
            auto newPos=new Position(ps);
            newPos->move=new MoveItem(move,x0,x1);
            swap_(newPos->board[x0],newPos->board[x1]);
            //Add to list
            PosList->AddItem(newPos);
        } while (type==_king);
        //restore incoming position
        ChIt->pos=x0;
        ChIt->type=type;
    }
    return res;
}

bool GameFunctions::MakeMovesList(Position *ps, ItemsList<Position> *PosList)
{
    bool res=false;
    auto tmp=ps->ChList[ps->ClrOfMove];
    tmp->SetToStart();
    while (tmp->CurrentItem())
    {
        res|=TryToKill(ps,PosList);
        tmp->SetToNext();
    }
    if (res) return res;// exit if Kill possible
    tmp->SetToStart();
    while (tmp->CurrentItem())
    {
        res|=TryToMove(ps,PosList);
        tmp->SetToNext();
    }
    return res;
}



QString Pos2Str(int pos, int Size)
{
    QString res;
    int8_t x[2];
    x[0]='A'+pos%Size;
    x[1]='1'+pos/Size;
    res=x[0];
    res+=x[1];
    return res;
}

MoveTreeItem::MoveTreeItem(MoveItem *move)
{
    this->move=move;
    next=new ItemsList <MoveTreeItem>;
}

QString MoveTreeItem::toString(uint8_t Size)
{
    return (move->kill<0)?Pos2Str(move->from, Size)+"-"+Pos2Str(move->to, Size):
                          Pos2Str(move->from, Size)+":"+Pos2Str(move->to, Size);
}

MoveItem::MoveItem(MoveItem *conn, int8_t from, int8_t to, int8_t kill):conn(conn),from(from),to(to),kill(kill)
{
    PtrCnt=0;
    if (conn) conn->PtrCnt++;
}

MoveItem::~MoveItem()
{
    if (conn && conn->PtrCnt)
        if (!(--conn->PtrCnt)) delete conn;
}

bool MoveItem::operator ==(const MoveItem &move)
{
    return (from==move.from && to==move.to && kill==move.kill);
}

QJsonObject Position::toJson()
{
    const QString _tp[2]={"simple","king"};
    const QString _clr[2]={"black","white"};
    QJsonArray ch;
    if (ChList[0]->Count<12 || ChList[1]->Count<12) qDebug () << "!!!!!" << this;
    for (auto it=ChList[_black]->begin();it;it=ChList[_black]->next())
        ch.append(QJsonObject({{"pos",it->pos},{"type",_tp[it->type]},{"color",_clr[it->color]}}));
    for (auto it=ChList[_white]->begin();it;it=ChList[_white]->next())
        ch.append(QJsonObject({{"pos",it->pos},{"type",_tp[it->type]},{"color",_clr[it->color]}}));
    QJsonObject res;
    res["ch"]=ch;
    return res;
}

Position::Position(int8_t Size, ItemsList<CheckerXY> *CheckersList, checker_color nextMoveColor)
{
    len=Size*Size;
    this->Size=Size;
    board = new ItemOfList <Ch>* [len];
    ChList[0]=new ItemsList <Ch>;
    ChList[1]=new ItemsList <Ch>;

    fill0(&board[0],len);

    this->ClrOfMove=nextMoveColor;
    CheckersList->SetToStart();
    while (CheckersList->CurrentItem())
    {
        Ch * ch=new Ch;
        ch->pos=CheckersList->CurrentItem()->y*Size+CheckersList->CurrentItem()->x;
        ch->type=CheckersList->CurrentItem()->type;
        ch->color=CheckersList->CurrentItem()->color;
        board[ch->pos]=ChList[CheckersList->CurrentItem()->color]->AddItem(ch);
        CheckersList->SetToNext();
    }
}

Position::Position(int8_t Size, checker_color nextMoveColor)
{
    len=Size*Size;
    this->Size=Size;
    board = new ItemOfList <Ch>* [len];
    ChList[0]=new ItemsList <Ch>;
    ChList[1]=new ItemsList <Ch>;

    fill0(&board[0],len);
    ClrOfMove=nextMoveColor;
}

Position::Position(Position *pos)//copy of object
{
    len=pos->len;
    Size=pos->Size;
    ClrOfMove=pos->ClrOfMove;
    board = new ItemOfList <Ch>* [len];
    ChList[0]=new ItemsList <Ch>;
    ChList[1]=new ItemsList <Ch>;
    fill0(&board[0],len);
    auto CurrItem0=pos->ChList[0]->Curr;
    auto CurrItem1=pos->ChList[1]->Curr;
    for (int i=0;i<2;i++)
    {
        if (!pos->ChList[i]->Count) continue;
        pos->ChList[i]->SetToStart();
        auto tmp=pos->ChList[i]->CurrentItem();
        while (tmp)
        {
            board[tmp->pos]=ChList[i]->AddItem(tmp);
            pos->ChList[i]->SetToNext();
            tmp=pos->ChList[i]->CurrentItem();
        }
    }
    pos->ChList[0]->Curr=CurrItem0;
    pos->ChList[1]->Curr=CurrItem1;
}

QString Position::PosToStr(int8_t pos)
{
    QString res;
    int8_t x[2];
    x[0]='A'+pos%Size;
    x[1]='1'+pos/Size;
    res=x[0];
    res+=x[1];
    return res;
}

QString Position::MoveAsStr()
{
    QString res;
    MoveItem *x=move;
    while (x)
    {
        int8_t x0=x->from;
        int8_t x1=x->kill;
        int8_t x2=x->to;
        if (x1>=0) res=PosToStr(x0)+":"+PosToStr(x1)+":"+PosToStr(x2)+res;
        else res=PosToStr(x0)+"-"+PosToStr(x2)+res;
        x=x->conn;
        if (x) res=">"+res;
    }
    return res;
}

Position::~Position()
{
    delete board;
    if (ChList[0]) delete ChList[0];
    if (ChList[1]) delete ChList[1];
    // !!!        while (track) {auto tmp=track;track=track->prev;delete tmp;}
}

//ItemsList<MoveTreeItem> *PTreeMoves::MoveVariantsFromPos(int8_t pos)
//{
//    if (pos==OldPos) return top;
//    OldPos=pos;
//    if (FirstStep)
//    {
//        FirstStep=false;
//        top->SetToStart();
//        Variants->ClearList();
//        while (top->CurrentItem()) {
//            if (top->CurrentItem()->move->from==pos) {
//                Variants->AddItem(top->CurrentItem());
//            }
//            top->SetToNext();
//        }
//        top=Variants;
//        if (!top->Count) return NULL;
//        return top;
//    } else
//    {
//        top->SetToStart();
//        while (top->Curr)
//        {
//            if (top->CurrentItem()->move->to==pos) {
//                top->CurrentItem()->prev=top;
//                movesTrack->AddItem(top->CurrentItem());
//                top=top->CurrentItem()->next;
//                if (!top->Count) return NULL;
//                return top;
//            }

//            top->SetToNext();
//        }
//    }
//    return NULL;
//}

Game::Game(int8_t size):size(size),routs(size)
{
    Pos=new Position(size,_white);
    StartPos();
}

void Game::StartPos()
{

    Pos->ChList[_black]->ClearList();
    Pos->ChList[_white]->ClearList();
    int rows=size/2;
    int ii;
    for (int8_t i=0;i<size;i++)
        for (int8_t j=0;j<size;j++)
        {
            ii=i*size+j;
            if ((i+j)%2) continue;
            if (i<(rows-1))
                Pos->board[ii]=Pos->ChList[_white]->AddItem(new Ch(ii,_simple,_white));
            if (i>rows)
                Pos->board[ii]=Pos->ChList[_black]->AddItem(new Ch(ii,_simple,_black));
        }
}

void Game::TrackLoop(int8_t from_pos)
{

    //        QJsonObject (*(xSocketThread::CallFunct) (QString, QJsonObject)
    if (false) return;//animation or client busy
    int trkcnt;
    PTreeMoves * TreeMoves;
    ItemsList <Position> * posList = new ItemsList <Position>;

    routs.MakeMovesList(Pos, posList);
    TreeMoves=new PTreeMoves(posList);
    ItemsList <MoveTreeItem> * top;
    auto XX=TreeMoves;
    do
    {
        trkcnt=XX->track->Count;
        if (last_points->Count==1)
            top=XX->MoveVariants(last_points->Last->It->pos); else
            top=XX->MoveVariants(from_pos);
        if (top && top->Count)// RefreshPoints(top);//вызвать обновление точек на клиенте
        {
            QJsonArray points;
            for (auto it=top->begin();it;it=top->next())
            {
                points.append(it->move->toJson());
            }
            auto res=CallFunct("GreenPoints",QJsonObject({{"points", points}}));
        }
        if (trkcnt<XX->track->Count)
        {
            auto xx=XX->track->Last->It->move;
            auto ChX=Pos->board[xx->from]->It;
            //                foreach (QGraphicsItem * i, scene->items()) i->setZValue(0);
            //                ChX->setZValue(1);
            //                Анимация шашки
            //                board->ChAnimation->LineMoveAnimation
            //                        (ChX,ChX->Xpos(xx->to), ChX->Ypos(xx->to), 100, 700);
            //            qDebug () << "animation start"<< ChX->_X() << ChX->_Y();
            ChX->pos=xx->to;
            if (is_king(Pos,xx->to))
            {
                ChX->type=_king;
                //                    ChX->UpdatePixmap(); обновить шашку на клиенте
            }
            if (xx->kill>=0)
            {
                auto yy=Pos->board[xx->kill]->It;
                XX->to_delete->AddItem(yy);//убитая шашка обновить у клиента

            }
            if (XX->Wait_for_First) {
                for (auto it=XX->to_delete->begin();it;it=XX->to_delete->next())
                {
                    auto xCh=Pos->board[it->pos];
                    Pos->ChList[it->color]->Delete(xCh);
                    Pos->board[it->pos]=NULL;
                }
                //                    RefreshPoints(top, true); обновить точки у клиента, удалить убитые шашки

                //                    XX->to_delete->SetToStart();
                //                    while (XX->to_delete->Curr)
                //                    {
                //                        board->CheckersList->
                //                                Delete(board->CheckersList->searchIt(XX->to_delete->CurrentItem()));
                //                        XX->to_delete->SetToNext();
                //                        board->Draw();
                //                    }
                Pos->ClrOfMove=reverse(Pos->ClrOfMove);
                return;
            }
        }

    } while (last_points->Count==1);
}

QJsonObject Game::CallFunct(QString functName)
{
    QJsonObject xx;
    return CallFunct(functName, xx);
}

QJsonObject Game::CallFunct(QString functName, QJsonObject args)
{
    return emit Call(functName, args);
}

CheckerXY::CheckerXY(checker_color color, checker_type type, uint8_t x, uint8_t y)
{
    this->x=x;
    this->y=y;
    this->type=type;
    this->color=color;
}

bool CheckerXY::operator ==(const CheckerXY &ch)
{
    return (x==ch.x && y==ch.y && type==ch.type && color==ch.color);
}

ItemsList<MoveTreeItem> *PTreeMoves::MoveVariants(int8_t pos)
{
    if (!track->Count)
    {
        for (auto it=saveTop->begin();it;it=saveTop->next())
            if (it->move->from==pos)
            {
                top=saveTop;
                Wait_for_First=true;
                break;
            }
    }
    if (Wait_for_First)
    {
        to_delete->ClearList();
        Variants->ClearList();
        for (auto it=top->begin();it;it=top->next())
            if (it->move->from==pos) Variants->AddItem(it);
        if (Variants->Count) {
            top=Variants;
            Wait_for_First=false;
        } else return nullptr;
        return top;
    } else
    {
        for (auto it=top->begin();it;it=top->next())
            if (it->move->to==pos)
            {
                track->AddItem(it);
                top=it->next;
                if (!top->Count) {
                    Wait_for_First=true;
                    return nullptr;
                }
                return top;
            }
    }
    return nullptr;
}

void PTreeMoves::DeleteTrack()
{
    track->ClearList();
    top=saveTop;
    Variants->ClearList();
}

PTreeMoves::~PTreeMoves()
{
    delete track;
    delete top;
    delete Variants;
    delete MoveList;
    delete to_delete;
}

PTreeMoves::PTreeMoves(ItemsList<Position> *pList)
{
    for (auto it=pList->begin();it;it=pList->next())
    {
        auto x=it->move;
        MoveItem * moveit=nullptr;
        MoveItem * nextIt=nullptr;
        while (x) {
            moveit=new MoveItem(nextIt,x->from,x->to,x->kill);
            x=x->conn;
            nextIt=moveit;
        }
        if (moveit) MoveList->AddItem(moveit);
    }
    for (auto it=MoveList->begin();it;it=MoveList->next())
    {
        auto x=it;
        auto tree=top;
        while (x)
        {
            tree->SetToStart();
            //search for MoveItem entering
            while (tree->Curr && !(*(tree->CurrentItem()->move)==*x)) tree->SetToNext();
            if (!tree->Curr) tree->AddItem(new MoveTreeItem(x));//Add new if not found entering
            x=x->conn;//set to next move-step of MoveList
            tree=tree->CurrentItem()->next;//jamp to next level of tree
        }
    }
    saveTop=top;
    track = new ItemsList <MoveTreeItem>;
}
