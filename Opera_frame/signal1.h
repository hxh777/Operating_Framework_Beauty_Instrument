#ifndef SIGNAL1_H
#define SIGNAL1_H

#include <QObject>
#include <QString>
#include <QDebug>

class Cpp1 : public QObject
{
    Q_OBJECT
    // 注册属性，通过Q_PROPERTY注册到元对象系统


signals:
    void tired(int minute);  // 累了

public slots:
    void walk() {  // 走路
        qDebug() << "Thirty thousand steps";
        emit tired(30);
    }
};














#endif // SIGNAL1_H
