#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QBluetoothSocket>
#include <QMessageBox>
#include <QListWidgetItem>

#include <QtBluetooth/qtbluetoothglobal.h>
//#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothsocket.h>

#include <QTextCodec>
#include <QDateTime>
#include <QTimer>//定时器
#include <QTime>
#include <QMovie>
#include "util.h"
#include <QTranslator>

#include <QUrl>
#include <QtQuickWidgets/QQuickWidget>

#include <signal1.h>
#include <QQmlEngine>
#include <QQmlComponent>


//#include <QQmlApplicationEngine>
//#include <QMetaObject>







namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void blueStates();

private:
    Ui::MainWindow *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;//用来对周围蓝牙进行搜寻
    QBluetoothLocalDevice *localDevice;//对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket1;//就是用来进行蓝牙配对链接和数据传输的
    QTimer *timer;
    QString fornt = "1";
    QString comStr;


    QTimer *timer_lcd;

    int processer_value = 0;
    //int test = 0;

    QString minute = "00:";
    QString seconds = "00";
    int min_value = 0;
    int sec_value = 0;
    QMovie *movie;

    QTranslator *My_Translator;
    int language;


    Cpp1 *cppqml;
protected:
    void changeEvent (QEvent *e);



signals:

    void switchLanguage(int index);





private slots:
     void on_pushButton_openBLE_clicked();
     void on_pushButton_upDateBLE_clicked();
     void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
     void findFinish();
     void connectBLE(QListWidgetItem *item);
     void connectOK();
     void connectNot();
     void readBluetoothDataEvent();
     void on_pushButton_closeBLE_clicked();
     void on_pushButton_clearReceive_clicked();
     void on_pushButton_send_clicked();
     void on_pushButton01_clicked();
     void on_pushButton_OK_clicked();
void ComboBox_activated(int index);







     void on_pushButton_p2_back_clicked();
     void on_pushButton_p2_Body_clicked();
     void on_pushButton_p3_back_clicked();
     void on_pushButton_topbody_clicked();
     void on_pushButton_p4_end_clicked();
     void on_pushButton_4_clicked();
     void on_pushButton_3_clicked();

     void on_pushButton_p4_begin_clicked();
     void on_pushButton_p4_pause_clicked();

     void on_pushButton_p5_back_clicked();
     void on_pushButton_p3_thigh_clicked();
     void on_pushButton_p5_next_clicked();
     void on_dial_p4_text_valueChanged(int value);
};

#endif // MAINWINDOW_H
