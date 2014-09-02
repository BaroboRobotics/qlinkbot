#ifndef QLINKBOT_H__
#define QLINKBOT_H__

#include "qbarobo_global.h"

#ifndef Q_MOC_RUN
#include "baromesh/baromesh.hpp"
#endif
#include <memory>

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class QLinkbot;
class QLinkbotWorker;
class CLinkbot;
void linkbotAccelCallback(int millis, double x, double y, double z, QLinkbot* linkbot);
void linkbotButtonCallback(CLinkbot* linkbot, int button, int buttonDown);
void linkbotJointCallback(int millis, double j1, double j2, double j3, double j4, QLinkbot* linkbot);

class QBAROBOSHARED_EXPORT QLinkbot : public QObject
{
  Q_OBJECT
  public:
    QLinkbot(const QString&);
    void connectRobot();
    void disconnectRobot();
    int enableAccelEventCallback();
    int enableButtonCallback();
    int enableJointEventCallback();
    QString getSerialID() const {return id_;}

    void lock();
    void unlock();

    inline bool operator==(const QLinkbot& other) { 
      return this->getSerialID() == other.getSerialID();
    }
    inline bool operator!=(const QLinkbot& other){return !operator==(other);}

    int setJointSpeeds (double, double, double);
    int disableAccelEventCallback ();
    int disableButtonCallback ();
    int disableJointEventCallback ();
    int getJointAngles (double, double, double, int=10);
    int moveNB (double, double, double);
    int moveToNB (double, double, double);
    int setColorRGB (int, int, int);
    int setJointEventThreshold (int, double);
    int stop ();
    int setBuzzerFrequencyOn (int);
    int getVersions (uint32_t&, uint32_t&, uint32_t&);

  signals:
    void buttonChanged(QLinkbot *linkbot, int button, int event);
    void jointsChanged(QLinkbot *linkbot, double j1, double j2, double j3, int mask);
    void jointChanged(QLinkbot *linkbot, int joint, double anglePosition);
    void accelChanged(QLinkbot *linkbot, double x, double y, double z);

  public slots:
    void newAccelValues(double x, double y, double z);
    void newButtonValues(int button, int buttonDown);
    void newMotorValues(double j1, double j2, double j3, int mask);

  private:
    QString id_;
    QMutex lock_;
    QWaitCondition cond_;
    QThread *workerthread_;
    QLinkbotWorker* worker_;

    std::unique_ptr<robot::Proxy> mProxy;
};

class QBAROBOSHARED_EXPORT QLinkbotWorker : public QObject
{
  Q_OBJECT
  public:
    QLinkbotWorker(QLinkbot *linkbot);
    void setNewAccelValues(double x, double y, double z);
    void setNewButtonValues(int button, int down);
    void setNewMotorValues(double j1, double j2, double j3, int mask);

  public slots:
    void doWork();

  signals:
    void accelChanged(double x, double y, double z);
    void buttonChanged(int button, int buttonDown);
    void motorChanged(double j1, double j2, double j3, int mask);

  private:
    QLinkbot* parentLinkbot_;
    QMutex lock_;
    QWaitCondition cond_;
    bool runflag_;
    bool accelValuesDirty_;
    double accel_[3];
    bool buttonValuesDirty_;
    int button_[2];
    bool motorValuesDirty_;
    double motor_[3];
    int motorMask_;
};

#endif
