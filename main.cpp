#include "linetranswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    lineTransWidget w;
    w.show();

    return a.exec();
}
