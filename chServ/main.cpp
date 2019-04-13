
#include <QtCore/QCoreApplication>

#include "server.h"
#include "game.h"

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    Server server(1234);
    return a.exec();
}
