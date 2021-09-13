#include "widget.h"
#include "opencv2/opencv.hpp"
#include <QApplication>

using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    SQLWidget w2;
//    w2.show();
    Widget w;
    w.show();
    return a.exec();
}
