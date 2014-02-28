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
  QLinkbot* l = (QLinkbot*)linkbot;
  emit l->buttonChanged(l, button, buttonDown);
}

void linkbotJointCallback(int , double j1, double j2, double j3, double , void *linkbot)
{
  QLinkbot* l = (QLinkbot*)linkbot;
  emit l->motorChanged(l, j1, j2, j3);
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
  QMetaObject::invokeMethod(worker_, "doWork", Qt::QueuedConnection); 
}

int QLinkbot::enableAccelEventCallback()
{
  return CLinkbot::enableAccelEventCallback(worker_, linkbotAccelCallback);
}

int QLinkbot::enableButtonCallback()
{
  return CMobot::enableButtonCallback(this, linkbotButtonCallback);
}

int QLinkbot::enableJointEventCallback()
{
  return CMobot::enableJointEventCallback(this, linkbotJointCallback);
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

QLinkbotWorker::QLinkbotWorker(QLinkbot* linkbot)
{
  parentLinkbot_ = linkbot;
  runflag_ = true;
}

void QLinkbotWorker::doWork()
{
  while(runflag_) {
    lock_.lock();
    while(
        runflag_ &&
        (accelValuesDirty_ == false)
        ) 
    {
      cond_.wait(&lock_);
    }
    if(accelValuesDirty_) {
      emit accelChanged(accel_[0], accel_[1], accel_[2]);
      accelValuesDirty_ = false;
    }
    lock_.unlock();
  }
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
