#ifndef QLINKBOT_H__
#define QLINKBOT_H__

#include <linkbot.h>
#include "qbarobo_global.h"

#include <QObject>
#include <QMutex>


class QLinkbot;
void linkbotAccelCallback(int millis, double x, double y, double z, QLinkbot* linkbot);
void linkbotButtonCallback(CLinkbot* linkbot, int button, int buttonDown);
void linkbotJointCallback(int millis, double j1, double j2, double j3, double j4, QLinkbot* linkbot);

class QBAROBOSHARED_EXPORT QLinkbot : public QObject, public CLinkbot
{
  Q_OBJECT
  public:
    int enableAccelEventCallback();
    int enableButtonCallback();
    int enableJointEventCallback();

    void lock();
    void unlock();

  signals:
    void buttonChanged(QLinkbot & linkbot, int button, int event);
    void motorChanged(QLinkbot & linkbot, double j1, double j2, double j3);
    void accelChanged(QLinkbot & linkbot, double x, double y, double z);

  private:
    QMutex lock_;
};

#endif
