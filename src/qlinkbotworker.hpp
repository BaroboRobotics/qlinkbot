#ifndef QLINKBOT_QLINKBOTWORKER_HPP
#define QLINKBOT_QLINKBOTWORKER_HPP

#include <QMutex>
#include <QObject>
#include <QWaitCondition>

#include <atomic>

struct QLinkbot;

class QLinkbotWorker : public QObject
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