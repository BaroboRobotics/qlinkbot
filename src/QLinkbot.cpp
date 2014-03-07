#include "QLinkbot.h"
#include <iostream>
#include <QThread>
#include <QDebug>

void linkbotAccelCallback(int , double x, double y, double z, void* worker)
{
  QLinkbotWorker* l = (QLinkbotWorker*)worker;
  l->setNewAccelValues(x, y, z);
}

void linkbotButtonCallback(void* linkbot, int button, int buttonDown)
{
  QLinkbotWorker* l = (QLinkbotWorker*)linkbot;
  l->setNewButtonValues(button, buttonDown);
}

void linkbotJointCallback(int , double j1, double j2, double j3, double , void *linkbot)
{
  QLinkbotWorker* l = (QLinkbotWorker*)linkbot;
  l->setNewMotorValues(j1, j2, j3);
}

QLinkbot::QLinkbot()
{
  worker_ = new QLinkbotWorker(this);
  workerthread_ = new QThread();
  worker_->moveToThread(workerthread_);
  workerthread_->start();
  QObject::connect(worker_, SIGNAL(accelChanged(double, double, double)),
      this, SLOT(newAccelValues(double, double, double)),
      Qt::QueuedConnection);
  QObject::connect(worker_, SIGNAL(buttonChanged(int, int)),
      this, SLOT(newButtonValues(int, int)));
  QObject::connect(worker_, SIGNAL(motorChanged(double, double, double)),
      this, SLOT(newMotorValues(double, double, double)));
  QMetaObject::invokeMethod(worker_, "doWork", Qt::QueuedConnection); 
}

void QLinkbot::connect(const QString & id)
{
  int rc;
  rc = connectWithAddress(static_cast<const char*>(id.toLatin1().constData()), 1);
  if(rc) {
    throw QString("Could not connect to the following robot: ") + id;
  }
  id_ = id;
}

int QLinkbot::enableAccelEventCallback()
{
  return CLinkbot::enableAccelEventCallback(worker_, linkbotAccelCallback);
}

int QLinkbot::enableButtonCallback()
{
  return CMobot::enableButtonCallback(worker_, linkbotButtonCallback);
}

int QLinkbot::enableJointEventCallback()
{
  return CMobot::enableJointEventCallback(worker_, linkbotJointCallback);
}

void QLinkbot::lock()
{
  lock_.lock();
}

void QLinkbot::unlock()
{
  lock_.unlock();
}

void QLinkbot::newAccelValues(double x, double y, double z)
{
  emit accelChanged(this, x, y, z);
}

void QLinkbot::newButtonValues(int button, int buttonDown)
{
  emit buttonChanged(this, button, buttonDown);
}

void QLinkbot::newMotorValues(double j1, double j2, double j3)
{
  emit jointChanged(this, j1, j2, j3);
}

QLinkbotWorker::QLinkbotWorker(QLinkbot* linkbot)
{
  parentLinkbot_ = linkbot;
  runflag_ = true;
}

void QLinkbotWorker::setNewAccelValues(double x, double y, double z)
{
  lock_.lock();
  accel_[0] = x;
  accel_[1] = y;
  accel_[2] = z;
  accelValuesDirty_ = true;
  cond_.wakeAll();
  lock_.unlock();
}

void QLinkbotWorker::setNewButtonValues(int button, int down)
{
  lock_.lock();
  button_[0] = button;
  button_[1] = down;
  buttonValuesDirty_ = true;
  cond_.wakeAll();
  lock_.unlock();
}

void QLinkbotWorker::setNewMotorValues(double j1, double j2, double j3)
{
  lock_.lock();
  motor_[0] = j1;
  motor_[1] = j2;
  motor_[2] = j3;
  motorValuesDirty_ = true;
  cond_.wakeAll();
  lock_.unlock();
}

void QLinkbotWorker::doWork()
{
  while(runflag_) {
    lock_.lock();
    while(
        runflag_ &&
        (accelValuesDirty_ == false) &&
        (buttonValuesDirty_ == false) &&
        (motorValuesDirty_ == false)
        ) 
    {
      cond_.wait(&lock_);
    }
    if(accelValuesDirty_) {
      emit accelChanged(accel_[0], accel_[1], accel_[2]);
      accelValuesDirty_ = false;
    }
    if(buttonValuesDirty_) {
      emit buttonChanged(button_[0], button_[1]);
      buttonValuesDirty_ = false;
    }
    if(motorValuesDirty_) {
      emit motorChanged(motor_[0], motor_[1], motor_[2]);
      motorValuesDirty_ = false;
    }
    lock_.unlock();
  }
}

//#include "moc_QLinkbot.cpp"
