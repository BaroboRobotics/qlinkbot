#include "QBaroboBridge.h"
#include "QLinkbot.h"


QBaroboBridge::QBaroboBridge()
{
}

void QBaroboBridge::angularSpeed(QString id, double s1, double s2, double s3)
{
}

void QBaroboBridge::disconnect(QString id)
{
}

void QBaroboBridge::move(QString id, double j1, double j2, double j3)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->move(j1, j2, j3);
}

void QBaroboBridge::stop(QString id)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->stop();
}

void QBaroboBridge::beginScan(QString id)
{
}

int QBaroboBridge::connect(QString id)
{
  QLinkbot *l = new QLinkbot();
  l->connect(id);
  QObject::connect(l, SIGNAL(buttonChanged(QLinkbot*, int, int)),
      this, SLOT(robotButtonEvent(QLinkbot*, int, int)));
  QObject::connect(l, SIGNAL(jointChanged(QLinkbot*, double, double, double)),
      this, SLOT(robotJointEvent(QLinkbot*, double, double, double)));
  QObject::connect(l, SIGNAL(accelChanged(QLinkbot*, double, double, double)),
      this, SLOT(robotAccelEvent(QLinkbot*, double, double, double)));
  l->enableAccelEventCallback();
  l->enableButtonCallback();
  l->enableJointEventCallback();
  linkbots_.push_back(l);
}

int QBaroboBridge::numConnectedRobots()
{
  return linkbots_.size();
}

QString QBaroboBridge::getRobotId(int index)
{
  return linkbots_[index]->getID();
}

void QBaroboBridge::robotButtonEvent(QLinkbot *l, int button, int event)
{
  emit buttonChanged(l->getSerialID(), button, event);
}

void QBaroboBridge::robotJointEvent(QLinkbot *l, double j1, double j2, double j3)
{
  emit motorChanged(l->getSerialID(), j1, j2, j3);
}

void QBaroboBridge::robotAccelEvent(QLinkbot *l, double x, double y, double z)
{
  emit accelChanged(l->getSerialID(), x, y, z);
}

QLinkbot* QBaroboBridge::getRobot(QString id)
{
  int i;
  for(i = 0; i < linkbots_.size(); i++) {
    if(linkbots_.at(i)->getSerialID() == id) {
      return linkbots_[i];
    }
  }
  return NULL;
}
