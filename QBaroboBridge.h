#ifndef QBAROBOBRIDGE_H
#define QBAROBOBRIDGE_H

#include "qbarobo_global.h"

#include <QObject>

class QBAROBOSHARED_EXPORT QBaroboBridge : public QObject
{
    Q_OBJECT
public:
    QBaroboBridge();

    Q_INVOKABLE void angularSpeed(QString id, double s1, double s2, double s3);
    Q_INVOKABLE void disconnect(QString id);
    Q_INVOKABLE void move(QString id, double j1, double j2, double j3);
    Q_INVOKABLE void stop(QString id);
    Q_INVOKABLE void beginScan(QString id);
    Q_INVOKABLE int connect(QString id);
    Q_INVOKABLE int numConnectedRobots();
    Q_INVOKABLE QString getRobotId(int index);

signals:
    void buttonChanged(QString id, int button, int event);
    void motorChanged(QString id, double j1, double j2, double j3);
    void accelChanged(QString id, double x, double y, double z);
    void idScanned(QString newId);
};

#endif // QBAROBOBRIDGE_H
