#include "QLinkbot.h"

void linkbotAccelCallback(int , double x, double y, double z, void* linkbot)
{
  QLinkbot* l = (QLinkbot*)linkbot;
  emit l->accelChanged(*l, x, y, z);
}

void linkbotButtonCallback(void* linkbot, int button, int buttonDown)
{
  QLinkbot* l = (QLinkbot*)linkbot;
  emit l->buttonChanged(*l, button, buttonDown);
}

void linkbotJointCallback(int , double j1, double j2, double j3, double , void *linkbot)
{
  QLinkbot* l = (QLinkbot*)linkbot;
  emit l->motorChanged(*l, j1, j2, j3);
}

int QLinkbot::enableAccelEventCallback()
{
  return CLinkbot::enableAccelEventCallback(this, linkbotAccelCallback);
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
