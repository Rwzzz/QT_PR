#include "widget.h"
#include "ui_widget.h"

#include <QThread>
using namespace CVS;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
      //定义模型文件D:\WorkPlace\QT\test1\hyperlpr_pri\model
    prc = new PipelinePR("D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\cascade.xml",
                         "D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\HorizonalFinemapping.prototxt","D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\HorizonalFinemapping.caffemodel",
                         "D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\Segmentation.prototxt","D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\Segmentation.caffemodel",
                         "D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\CharacterRecognization.prototxt","D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\CharacterRecognization.caffemodel",
                         "D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\SegmenationFree-Inception.prototxt","D:\\WorkPlace\\QT\\test1\\hyperlpr_pri\\model\\SegmenationFree-Inception.caffemodel"
                       );


    db = QSqlDatabase::addDatabase("QMYSQL");//加入mysql数据库
    db.setHostName("localhost");
    db.setDatabaseName("first");
    //打开数据库
    if(db.open("root","root")==false)
    {
        QMessageBox::warning(this,"wrong",db.lastError().text());
        return;
    }

    QSqlQuery query("show tables from first");//数据库里的所有表
    while (query.next()) {
        QString country = query.value(0).toString();
        qDebug()<<country;
    }
    model = new QSqlTableModel(this,db);
    ui->tableView_show_sql->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView_show_sql->item(row, column)->setBackground(QBrush(QColor(255, 0, 0)));
    ui->label_show_vio->setScaledContents(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(Read_Frame()));
}

Widget::~Widget()
{
    delete prc;
    delete ui;
}
void Widget::Recognition(cv::Mat &image)
{

    QTime Time;
    Time.start();
    std::vector<pr::PlateInfo> res = prc->RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);
    float Used_Time =  Time.elapsed()/1000.0;
    //使用端到端模型模型进行识别 识别结果将会保存在res里面
    int i = 0;
    for(auto st:res) {
        if(st.confidence>0.9) {
            qDebug() << QString(st.getPlateName().data()).toUtf8().data() << " " << st.confidence ;
            int length = st.getPlateName().length();
            if(length!=9)
                return;
            time_t t = time(0);
            char tmp[64];
            strftime( tmp, sizeof(tmp), "%Y-%m-%d %X",localtime(&t) );
            /* 画车牌 */
            cv::Rect region = st.getPlateRect();
            cv::rectangle(image,region,Scalar(0,0,255),1,1,0);
            //qDebug()<<region.x <<region.y << region.width << region.height <<st.getPlateType();
            /* 入数据库 */
            opt_db(QString("insert into %1(time,plate) values('%2','%3')").arg("record").arg( tmp).arg(st.getPlateName().data()));

            QString plate_type;
            switch (st.getPlateType()) {
                case BLUE:
                    plate_type="蓝牌";
                    break;
                case GREEN:
                    plate_type="绿牌";
                    break;
                case YELLOW:
                    plate_type="黄牌";
                    break;
                case WHITE:
                    plate_type="白牌";
                    break;
                default:
                    plate_type="UNKNOWN";
                    break;

            }
            if(i == 0)
            {
                ui->label_show_result->setText(QString("%1 车牌号：%2 识别率：%3 耗时：%4 s ").arg(plate_type).arg(QString(st.getPlateName().data()).toUtf8().data()).arg(st.confidence).arg(Used_Time));
                i=1;
            }
            else{

                ui->label_show_vio->clear();
                QString text = ui->label_show_result->text();
                ui->label_show_result->setText(QString(text+'\n'+"%1 车牌号：%2 识别率：%3 耗时：%4 s ").arg(plate_type).arg(QString(st.getPlateName().data()).toUtf8().data()).arg(st.confidence).arg(Used_Time));
                i++;
            }
        }
    }
    QSize size = ui->label_show_vio->size();
    cv::resize(image,image,Size(size.rwidth(),size.rheight()));
    ui->label_show_vio->setPixmap(QPixmap::fromImage(cvMatToQImage(image)));
}



void Widget::opt_db(QString cmd)
{
    QSqlQuery query;
    if(query.exec(cmd))
    {
        qDebug() << "执行成功！";
    }
    else{
        qDebug() << query.lastError();
    }

}



void Widget::on_Box_Fir_Choose_currentIndexChanged(const QString &arg1)
{

}

void Widget::on_Box_Sec_Choose_currentIndexChanged(const QString &arg1)
{

}

void Widget::on_btn_commit_clicked()
{
    model->database().transaction();
    if(model->submitAll()){
        if(model->database().commit()){
            QMessageBox::information(this,"tableModel","数据修改成功！");
        }
    }
    else{
        model->database().rollback();
        QMessageBox::warning(this,"tableModel",tr("数据库错误：%1").arg(model->lastError().text()));
    }

}

void Widget::on_btn_select_clicked()
{
    QString text = ui->Box_Sec_Choose->currentText();
    if(text == QString("用户表"))
    {
        this->model->setTable("user");
        ui->tableView_show_sql->setModel(model);
    }else if(text == QString("车牌表")){
        this->model->setTable("record");
        ui->tableView_show_sql->setModel(model);
    }else if(text == QString("车辆次数统计信息")){
        QSqlQuery query;
        query.exec("truncate table statistics");

        QSqlQuery query1;
        QSqlQuery query2;
        query.exec("select * from user");//数据库里的所有表
        while (query.next()) {
            QString name = query.value(0).toString();
            QString plate = query.value(1).toString();
            qDebug()<< name<<plate;
            query1.exec(QString("select count(*) from record where plate='%1'").arg(plate));//数据库里的所有表
            qDebug()<<query1.size();
            if(query1.size()!= -1)
            {
                query1.first();
                int count = query1.value(0).toInt();
                qDebug()<<count;
                query2.exec(QString("insert into statistics(name,plate,count) values('%1','%2','%3')").arg(name).arg( plate).arg(count));

            }

        }

        this->model->setTable("statistics");
        ui->tableView_show_sql->setModel(model);
    }
    this->model->select();
}

void Widget::on_btn_insert_clicked()
{
    QModelIndex CurIndex=ui->tableView_show_sql->currentIndex();
    model->insertRow(CurIndex.row()+1,QModelIndex());
}

void Widget::on_btn_delete_clicked()
{
    int curRow=ui->tableView_show_sql->currentIndex().row();

    int ok=QMessageBox::warning(this,"删除当前行","你确定要删除？",QMessageBox::Yes,QMessageBox::No);

    if(ok==QMessageBox::Yes)
    {
        model->removeRow(curRow);
        model->select();
    }
}

void Widget::on_btn_backout_clicked()
{
    model->revertAll();
}

void Widget::on_image_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open file"),
                                                    "./",
                                                    tr("图片文件(*png *jpg);;"
                                                    ));
    if(!fileName.isEmpty())
    {
        qDebug() <<fileName;
        picture = cv::imread(fileName.toUtf8().data());
        Recognition(picture);

    }

}

void Widget::on_video_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open file"),
                                                    "./",
                                                    tr("视频文件(*mp4 *avi);;"
                                                    ));
    qDebug() << fileName;
    if(!fileName.isEmpty())
    {
        capture =  cv::VideoCapture(fileName.toUtf8().data());
        int fps = capture.get(CAP_PROP_FPS);
        qDebug()<<"fps:" << fps;
        timer.start(1000/fps+100);
        qDebug() << capture.isOpened();

    }
}
void Widget::Read_Frame()
{
    if(capture.isOpened())
    {
        cv::Mat Frame ;
        bool result = capture.read(Frame);

        if(!result)
        {
            capture.release();
            timer.stop();
        }else
            Recognition(Frame);
    }
}

void Widget::on_pushButton_clicked()
{
    capture.release();
    timer.stop();
}

void Widget::on_camera_clicked()
{
    m_camera.clear();

    bool res = capture.open(1);
    qDebug() << capture.isOpened();
    if(res)
    {
        int fps = capture.get(CAP_PROP_FPS);
        qDebug()<<"fps:" << fps;
        timer.start(1000/fps+100);
        qDebug() << capture.isOpened();
    }
}
