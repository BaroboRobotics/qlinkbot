#ifndef QLINKBOT_H__
#define QLINKBOT_H__

#include <linkbot.h>
#include "qbarobo_global.h"

#include <QObject>
#include <QMutex>
#include <QWaitCondition>


class QLinkbot;
class QLinkbotWorker;
void linkbotAccelCallback(int millis, double x, double y, double z, QLinkbot* linkbot);
void linkbotButtonCallback(CLinkbot* linkbot, int button, int buttonDown);
void linkbotJointCallback(int millis, double j1, double j2, double j3, double j4, QLinkbot* linkbot);

class QBAROBOSHARED_EXPORT QLinkbot : public QObject, public CLinkbot
{
  Q_OBJECT
  public:
    QLinkbot();
    int enableAccelEventCallback();
    int enableButtonCallback();
    int enableJointEventCallback();

    void lock();
    void unlock();

  signals:
    void buttonChanged(QLinkbot *linkbot, int button, int event);
    void motorChanged(QLinkbot *linkbot, double j1, double j2, double j3);
    void accelChanged(QLinkbot *linkbot, double x, double y, double z);

  public slots:
    void newAccelValues(double x, double y, double z);

  private:
    QMutex lock_;
    QWaitCondition cond_;
    QThread *workerthread_;
    QLinkbotWorker* worker_;
};

class QLinkbotWorker : public QObject
{
  Q_OBJECT
  public:
    QLinkbotWorker(QLinkbot *linkbot);
    void setNewAccelValues(double x, double y, double z);

  public slots:
    void doWork();

  signals:
    void accelChanged(double x, double y, double z);

  private:
    QLinkbot* parentLinkbot_;
    QMutex lock_;
    QWaitCondition cond_;
    bool runflag_;
    bool accelValuesDirty_;
    double accel_[3];
};

#endif
