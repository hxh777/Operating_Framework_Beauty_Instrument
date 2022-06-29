#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>







static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");
/* 全局变量 方便控制端调用蓝牙连接成功后的socket发送数据 */
QBluetoothSocket *socket1;
#include <QPainter>
#include <QProxyStyle>

class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
        const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 100; // 设置每个tabBar中item的大小
            s.rheight() = 150;
        }
        return s;
    }
   // ~CustomTabStyle();

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x89cfff);
                   // painter->setBrush(QBrush(0x89cfff));
                    //painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);
                }
                else {
                    painter->setPen(0x5d5d5d);
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};






MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);






//蓝牙连接
{
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    timer = new QTimer();
    localDevice = new QBluetoothLocalDevice();
    //给socket分配内存，限定套接字协议
    socket1 = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    //发现设备时会触发deviceDiscovered信号，转到槽显示蓝牙设备
    connect(discoveryAgent,SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this,SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo)));
    connect(discoveryAgent,SIGNAL(finished()),this,SLOT(findFinish()));
    //双击listwidget的项目，触发连接蓝牙的槽
    connect(ui->listWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(connectBLE(QListWidgetItem*)));
    //蓝牙连接设备成功后会停止搜索设备，显示连接成功
    connect(socket1,SIGNAL(connected()),this,SLOT(connectOK()));
    //蓝牙连断开连接后，会显示已断开连接
    connect(socket1,SIGNAL(disconnected()),this,SLOT(connectNot()));
    //接受到上位机传来的数据后显示，会触发接受数据函数
    connect(socket1,SIGNAL(readyRead()),this,SLOT(readBluetoothDataEvent()));
}


//按钮控制数字时间定时器
{
    ui->lcdNumber->display("00:00");

    //用一个定时信号来更新
    timer_lcd=new QTimer(this);

    connect(timer_lcd, &QTimer::timeout, [=](){
       // QString qstr = QString::number(i,10,0);   // 改变i的值
       // ui->lcdNumber->display(qstr);               // 显示改变后的i值

        sec_value += 1;
        if(sec_value < 10){
             seconds = "0" + QString::number(sec_value);
        }
        else if (sec_value < 60) {
             seconds = QString::number(sec_value);
        }
        else {
             sec_value  = 0;
             seconds    = "00";
             min_value += 1;
        }
        if(min_value < 10){
            minute = "0" + QString::number(min_value) + ":";
        }
        else if(min_value <60){
            seconds = QString::number(min_value) + ":";
        }else {
            min_value = 0;
        }

        ui->lcdNumber->display(minute + seconds);

    });

}


    My_Translator = new QTranslator(this);
    connect(ui->comboBox_p1_language,SIGNAL(activated(int )),this,SLOT(ComboBox_activated(int )));
    connect(this,SIGNAL(switchLanguage(int )),this,SLOT(ComboBox_activated(int )));

    QString language_value;
    bool is_read = Util::readInit(QString("./user.ini"), QString("language"), language_value);
    if(is_read){
        emit  switchLanguage( language_value.toInt());
    }




    const QUrl url(QStringLiteral("qrc:/video/qmll.qml"));
    //ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget_thigh->setClearColor(QColor(Qt::transparent));
    ui->quickWidget_thigh->setSource(url);








    //gif播放
    movie = new QMovie(":/image/12.gif");
    ui->label_10->setMovie(movie);
    movie->start();

    ui->tabWidget_2->tabBar()->setStyle(new CustomTabStyle);









}

MainWindow::~MainWindow()
{
    delete ui;
}

//打开蓝牙并查找蓝牙设备

void MainWindow::on_pushButton_openBLE_clicked()
{
    if( localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff)//开机没有打开蓝牙
    {
        localDevice->powerOn();//调用打开本地的蓝牙设备
        discoveryAgent->start();//开始扫描蓝牙设备
    }
    else
    {
         QMessageBox::information(this, tr("成功"), tr("蓝牙已打开"));
    }

}

// 关闭 断开已连接的蓝牙设备 close设备和我们的open设备的方法在形式上不一样，只能用这样的方法对蓝牙进行关闭。
void MainWindow::on_pushButton_closeBLE_clicked()
{
    socket1->close();
    QMessageBox::information(this, tr("成功"), tr("已断开连接"));

}
//刷新 重新查找蓝牙设备
void MainWindow::on_pushButton_upDateBLE_clicked()
{
    discoveryAgent->start();
    ui->listWidget->clear();
}

//在ListWidget上显示查找到的蓝牙设备
void MainWindow::addBlueToothDevicesToList(const QBluetoothDeviceInfo &info)
{
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    QList<QListWidgetItem *> items = ui->listWidget->findItems(label, Qt::MatchExactly);

    if (items.empty())
    {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(info.address());
        /* 蓝牙状态pairingStatus，Pairing枚举类型
         * 0:Unpaired没配对
         * 1:Paired配对但没授权
         * 2:AuthorizedPaired配对且授权 */
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
            item->setTextColor(QColor(Qt::green));
        else
            item->setTextColor(QColor(Qt::black));
        ui->listWidget->addItem(item);
    }
}

//刷新完成
void MainWindow::findFinish()
{
    ui->pushButton_upDateBLE->setEnabled(true);
    QMessageBox::information(this, tr("刷新"), tr("刷新完成!"));
}
//蓝牙连接
void MainWindow::connectBLE(QListWidgetItem *item)
{
    QString text = item->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;
    QBluetoothAddress address(text.left(index));
    QString name(text.mid(index + 1));
    QMessageBox::information(this,tr("提示"),tr("设备正在连接中..."));
    socket1->connectToService(address, QBluetoothUuid(serviceUuid) ,QIODevice::ReadWrite);
}
//连接成功
void MainWindow::connectOK()
{
    discoveryAgent->stop();  //停止搜索设备
    QMessageBox::information(this, tr("成功"), tr("连接成功!"));
}
//连接失败
void MainWindow::connectNot()
{
    QMessageBox::information(this, tr("提示"), tr("已断开连接"));
}

//窗口显示串口传来的数据
void MainWindow::readBluetoothDataEvent()
{
    //这里数据需要自己做处理，不然发送接受不成功，提示：用定时器定时接受
    QByteArray line = socket1->readAll();
    QString strData = line.toHex();
    comStr.append(strData);
    if(comStr.length() >= 4)
    {
        ui->textBrowser_receive->append(comStr + "\n");
        comStr.clear();
    }
}

//蓝牙状态
void MainWindow::blueStates()
{
    if(socket1 == NULL)
    QMessageBox::information(this, "错误", "蓝牙未连接");
}

void MainWindow::on_pushButton_clearReceive_clicked()
{
    ui->textBrowser_receive->clear();
}

void MainWindow::on_pushButton_send_clicked()
{
    QByteArray arrayData;
    QString s("Welcome to pay attention to WeChat public number Guoguo young teachers\n");
    socket1->write(s.toUtf8());
}
void MainWindow::on_pushButton01_clicked()
{
    blueStates();
//    socket1->write(fornt.toLatin1());
    QByteArray  data;
    static uint8_t count = 0;
    data.resize(1);
    if(count%2==0){


    data[0]=0x01;

//    data[1]=0x02;

//    data[2]=0x03;

//    data[3]=0x00;

//    data[4]=0x04;
    }
    else if (count%2==1) {
        data[0] = 0x5A;

    }
    socket1->write(data,1);//4会少发一位

    count = count + 1;


}




void MainWindow::on_pushButton_OK_clicked()
{

    ui->stackedWidget->setCurrentIndex(1);

}


void MainWindow::on_pushButton_p2_back_clicked()
{

    ui->stackedWidget->setCurrentIndex(0);
}








void MainWindow::on_pushButton_p2_Body_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_p3_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_topbody_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_p4_end_clicked()
{
        ui->stackedWidget->setCurrentIndex(0);
        timer_lcd->stop();
        min_value = 0;
        sec_value = 0;
        ui->lcdNumber->display("00:00");

}

void MainWindow::on_pushButton_4_clicked()
{
    processer_value += 10;
    if(processer_value>100)
    {
        processer_value = 100;
    }
    ui->progressBar->setValue(processer_value);
}

void MainWindow::on_pushButton_3_clicked()
{
    processer_value -= 10;
    if(processer_value<0)
    {
        processer_value =0 ;
    }


    ui->progressBar->setValue(processer_value);
}

void MainWindow::on_pushButton_p4_begin_clicked()
{
     timer_lcd->start(1000);// 每1000ms更新一次
}

void MainWindow::on_pushButton_p4_pause_clicked()
{
     timer_lcd->stop();
}

void MainWindow::changeEvent (QEvent *e)
{
    ui->retranslateUi(this);

    switch(e->type()){
     case QEvent :: LanguageChange :
        qDebug()<<"gaibian"<<endl;
        break;
     default:
        break ;

    }
}

void MainWindow::ComboBox_activated(int index)
{
    qDebug()<<index<<endl;
    qDebug()<<ui->comboBox_p1_language->currentIndex()<<endl;
    if(index != ui->comboBox_p1_language->currentIndex()){
        ui->comboBox_p1_language->setCurrentIndex(index);
        qDebug()<<"test"<<endl;
    }



    switch (index) {

    case 0 :
        My_Translator->load("://res/Translate_zh");
        break;


    case 1 :
         My_Translator->load("://res/Translate_EN");
        break;

    default:
        break;
    }
    qApp->installTranslator(My_Translator);
    language = index ;

    Util::writeInit(QString("./user.ini"), QString("language"), QString::number(language, 10));
}


void MainWindow::on_pushButton_p3_thigh_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}




void MainWindow::on_pushButton_p5_back_clicked()
{
     ui->stackedWidget->setCurrentIndex(2);

     QQmlEngine eng;
     QQmlComponent compent(&eng,QUrl(QStringLiteral("qrc:/video/qmll.qml")));


     QObject *object = compent.create();
     QVariant temp;

     qDebug()<<"diyicihanshu";
     QMetaObject::invokeMethod(object,"one");
     qDebug()<<"diercihanshu";
     QMetaObject::invokeMethod(object,"two",Q_ARG(QVariant,QVariant("two")));

//     QMetaObject::invokeMethod(object,"three");






//       delete ui->quickWidget_thigh;




}



void MainWindow::on_pushButton_p5_next_clicked()
{
     ui->stackedWidget->setCurrentIndex(3);




}

void MainWindow::on_dial_p4_text_valueChanged(int value)
{
    blueStates();
    static int temp=31;
    QByteArray  data;
    data.resize(1);
    if(value - temp >= 0)
    {
        data[0]=0x01;
        socket1->write(data,1);//4会少发一位
        //qDebug()<<value<<endl;
    }
    else {
        data[0]=0x5A;
        socket1->write(data,1);//4会少发一位
       // qDebug()<<value<<endl;
    }
    temp = value;
}
