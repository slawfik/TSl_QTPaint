#include "tsl_qtpaint.h"
#include <QApplication>
#include <chrono>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TSL_QTPaint w;
    w.show();

    return a.exec();
}

