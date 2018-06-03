#include "dolaplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DolaPlayer w;
    w.show();
    return a.exec();
}
