#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "opencv2/opencv.hpp"
#include <QDebug>
#include <QString>
#include "include/Pipeline.h"
#include "CVMat_QPixmap.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QFileDialog>
#include <QRect>
#include <time.h>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QModelIndex>
#include <QCameraInfo>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
using namespace  cv;
using namespace pr;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void Recognition(cv::Mat &image);

private slots:
//    void on_btn_Choose_clicked();

//    void on_btn_recognition_clicked();

    void on_Box_Fir_Choose_currentIndexChanged(const QString &arg1);

    void on_Box_Sec_Choose_currentIndexChanged(const QString &arg1);

    void on_btn_commit_clicked();

    void on_btn_select_clicked();

    void on_btn_insert_clicked();

    void on_btn_delete_clicked();

    void on_btn_backout_clicked();

    void opt_db(QString cmd);
    void on_image_clicked();

    void on_video_clicked();
    void Read_Frame();

    void on_pushButton_clicked();

    void on_camera_clicked();

private:

    Ui::Widget *ui;
    QString fileName;
    pr::PipelinePR *prc;

    QSqlDatabase db;
    QSqlTableModel *model;
    QPixmap src_img;

    QList<QCameraInfo> m_camera;
    cv::VideoCapture capture;
    cv::VideoCapture camera;
    cv::Mat picture;
    QTimer timer;

};
#endif // WIDGET_H
