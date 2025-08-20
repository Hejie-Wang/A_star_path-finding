#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    //w.resize(800, 600);
    w.loadMapFile();
    // 打印地图
    //w.showMap();
    w.show();
    return a.exec();
}
