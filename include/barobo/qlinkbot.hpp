#ifndef QLINKBOT_H__
#define QLINKBOT_H__

#include "qbarobo_global.h"

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include <memory>
#include <atomic>

namespace robot {
    class Proxy;
}

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
    explicit QLinkbot(const QString&);
    ~QLinkbot ();

    // noncopyable
    QLinkbot (const QLinkbot&) = delete;
    QLinkbot& operator= (const QLinkbot&) = delete;

    // movable
    friend void swap (QLinkbot& lhs, QLinkbot& rhs);
    QLinkbot (QLinkbot&&);

    void connectRobot();
    void disconnectRobot();
    int enableAccelEventCallback();
    int enableButtonCallback();
    int enableJointEventCallback();
    QString getSerialID() const;

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
    int getJointAngles (double&, double&, double&, int=10);
    int moveNB (double, double, double);
    int moveToNB (double, double, double);
    int setColorRGB (int, int, int);
    int setJointEventThreshold (int, double);
    int stop ();
    int setBuzzerFrequencyOn (float);
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
    struct Impl;
    std::unique_ptr<Impl> m;
};

class QBAROBOSHARED_EXPORT QLinkbotWorker : public QObject
{
  Q_OBJECT
  public:
    QLinkbotWorker(QLinkbot *linkbot);
    ~QLinkbotWorker ();
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
    std::atomic<bool> runflag_ = { false };
    bool accelValuesDirty_;
    double accel_[3];
    bool buttonValuesDirty_;
    int button_[2];
    bool motorValuesDirty_;
    double motor_[3];
    int motorMask_;
};

#endif
