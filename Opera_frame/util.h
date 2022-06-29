
#ifndef UTIL_H
#define UTIL_H

#include <QSettings>

class Util
{

public:

    static bool writeInit(QString path, QString user_key, QString user_value);
    static bool readInit(QString path, QString user_key, QString &user_value);

};

#endif //UTIL_H
