#include "qlinkbotworker.hpp"

#if 0
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

void linkbotJointCallback(int , double j1, double j2, double j3, double , int mask, void *linkbot)
{
  QLinkbotWorker* l = (QLinkbotWorker*)linkbot;
  l->setNewMotorValues(j1, j2, j3, mask);
}
#endif


QLinkbotWorker::QLinkbotWorker(QLinkbot* linkbot)
{
  parentLinkbot_ = linkbot;
}

QLinkbotWorker::~QLinkbotWorker () {
  runflag_ = false;

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

void QLinkbotWorker::setNewMotorValues(double j1, double j2, double j3, int mask)
{
  lock_.lock();
  motor_[0] = j1;
  motor_[1] = j2;
  motor_[2] = j3;
  motorValuesDirty_ = true;
  motorMask_ |= mask;
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
      emit motorChanged(motor_[0], motor_[1], motor_[2], motorMask_);
      motorValuesDirty_ = false;
      motorMask_ = 0;
    }
    lock_.unlock();
  }
}