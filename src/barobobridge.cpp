#include "barobobridge.hpp"
#include "barobo/qlinkbot.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

BaroboBridge::BaroboBridge()
{
}

void BaroboBridge::addRobot(QLinkbot* l)
{
  QObject::connect(l, SIGNAL(buttonChanged(QLinkbot*, int, int)),
      this, SLOT(robotButtonEvent(QLinkbot*, int, int)));
  QObject::connect(l, SIGNAL(jointsChanged(QLinkbot*, double, double, double, int)),
      this, SLOT(robotJointsEvent(QLinkbot*, double, double, double, int)));
  QObject::connect(l, SIGNAL(jointChanged(QLinkbot*, int, double)),
      this, SLOT(robotJointEvent(QLinkbot*, int, double)));
  QObject::connect(l, SIGNAL(accelChanged(QLinkbot*, double, double, double)),
      this, SLOT(robotAccelEvent(QLinkbot*, double, double, double)));
  linkbots_.insert(l->getSerialID(), l);
}

void BaroboBridge::angularSpeed(QString id, double s1, double s2, double s3)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->setJointSpeeds(s1, s2, s3);
}

void BaroboBridge::beginScan(QString id)
{
}

int BaroboBridge::connectRobot(QString id)
{
  QLinkbot *l;
  l = getRobot(id);
  if(l) return 0;
  l = new QLinkbot();
  try {
    l->connectRobot(id);
  }
  catch (...) {
    return -1;
  }
  QObject::connect(l, SIGNAL(buttonChanged(QLinkbot*, int, int)),
      this, SLOT(robotButtonEvent(QLinkbot*, int, int)));
  QObject::connect(l, SIGNAL(jointChanged(QLinkbot*, int, double)),
      this, SLOT(robotJointEvent(QLinkbot*, int, double)));
  QObject::connect(l, SIGNAL(jointsChanged(QLinkbot*, double, double, double, int)),
      this, SLOT(robotJointsEvent(QLinkbot*, double, double, double, int)));
  QObject::connect(l, SIGNAL(accelChanged(QLinkbot*, double, double, double)),
      this, SLOT(robotAccelEvent(QLinkbot*, double, double, double)));
  linkbots_.insert(id, l);
  return 0;
}

void BaroboBridge::deleteRobot(QString id)
{
  linkbots_.remove(id);
}

void BaroboBridge::disconnectRobot(QString id)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->disconnectRobot();
  deleteRobot(id);
}

void BaroboBridge::enableAccelSignals(QString id, bool enable)
{
  QLinkbot* l = getRobot(id);
  if(l == NULL) {return;}
  if(enable) {
    l->enableAccelEventCallback();
  } else {
    l->disableAccelEventCallback();
  }
}

void BaroboBridge::enableButtonSignals(QString id, bool enable)
{
  qDebug() << "Setting button signals to: " << enable;
  QLinkbot* l = getRobot(id);
  if(l == NULL) {return;}
  if(enable) {
    l->enableButtonCallback();
  } else {
    l->disableButtonCallback();
  }
}

void BaroboBridge::enableMotorSignals(QString id, bool enable)
{
  QLinkbot* l = getRobot(id);
  if(l == NULL) {return;}
  if(enable) {
    l->enableJointEventCallback();
  } else {
    l->disableJointEventCallback();
  }
}

void BaroboBridge::fetch(QVariantMap urlInfo)
{
  QUrl url = urlInfo.value("url").toUrl();
  QString dest = urlInfo.value("destination").toString();
  /* FIXME: Memory leak */
  FetchTransaction* transaction = new FetchTransaction(url, dest);
  QObject::connect(transaction, SIGNAL(fetchFinished(QUrl, bool)),
      this, SLOT(forwardFetchFinished(QUrl, bool)));
  transaction->start();
}

QVariantList BaroboBridge::getMotorAngles(QString id)
{
  QVariantList list;
  QLinkbot *l = getRobot(id);
  if(!l) return list;
  double angle1, angle2, angle3;
  int rc = l->getJointAngles(angle1, angle2, angle3);
  if(rc) {
    return list;
  }
  list << QVariant(angle1) << QVariant(angle2) << QVariant(angle3);
  return list;
}

void BaroboBridge::move(QString id, double j1, double j2, double j3)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->moveNB(j1, j2, j3);
}

void BaroboBridge::moveTo(QString id, double j1, double j2, double j3)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->moveToNB(j1, j2, j3);
}

int BaroboBridge::numConnectedRobots()
{
  return linkbots_.size();
}

void BaroboBridge::setLEDColor(QString id, int r, int g, int b)
{
  QLinkbot* l = getRobot(id);
  if(NULL == l) return;
  l->setColorRGB(r, g, b);
}

void BaroboBridge::setMotorEventThreshold(QString id, int motor, double degrees)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->setJointEventThreshold(motor, degrees);
}

void BaroboBridge::stop(QString id)
{
  QLinkbot *l = getRobot(id);
  if(!l) return;
  l->stop();
}

void BaroboBridge::buzzerFrequency (QString id, double freq) {
  QLinkbot* l = getRobot(id);
  if (!l) return;
  l->setBuzzerFrequencyOn(freq);
}

QString BaroboBridge::firmwareVersion (QString id) {
  QLinkbot *l = getRobot(id);
  if (!l) return "";
  unsigned version = 0;
  l->getVersions(version);
  unsigned major = 0xff & (version >> (2 * CHAR_BIT));
  unsigned minor = 0xff & (version >> (1 * CHAR_BIT));
  unsigned patch = 0xff & (version >> (0 * CHAR_BIT));
  return QString("v%1.%2.%3").arg(
      QString::number(major),
      QString::number(minor),
      QString::number(patch));
}

QStringList BaroboBridge::availableFirmwareVersions () {
  /* Roughly equivalent to:
   *   find /path/to/barobobrowser/firmware -name "*.hex" -readable -type f \
   *      | sort | xargs -I '{}' basename '{}' .hex
   */

  QDir firmwareDir {
    firmwareDirectory(),          // path
    "*" + firmwareSuffix(),       // glob
    QDir::Name,                   // sort
    QDir::Files | QDir::Readable  // file type filter
  };

  QStringList firmwares;
  for (auto& f : firmwareDir.entryInfoList()) {
    firmwares.append(f.completeBaseName());
  }

  return firmwares;
}

#if 0
bool BaroboBridge::canFlashFirmware (QString id) {
  QLinkbot *l = getRobot(id);
  if (!l) return false;
  return l->canFlashFirmware();
}

int BaroboBridge::flashFirmware (QString id, QString version) {
  QLinkbot *l = getRobot(id);
  if (!l) return -1;

  QDir firmwareDir { firmwareDirectory() };
  QString firmwareFilename = version + firmwareSuffix();

  /* Check if the associated firmware hex file actually exists. */
  if (!firmwareDir.exists(firmwareFilename)) {
    return -1;
  }

  int rc = l->flashFirmwareAsync(
      firmwareDir.filePath(firmwareFilename).toStdString(),
      &BaroboBridge::flashFirmwareProgressEvent,
      &BaroboBridge::flashFirmwareCompletionEvent,
      static_cast<void*>(this));
  if (0 == rc) {
    /* FIXME this should be unnecessary with a disconnection event */
    deleteRobot(id);
  }
  return rc;
}

void BaroboBridge::flashFirmwareProgressEvent (double progress, void* user_data) {
  auto self = static_cast<BaroboBridge*>(user_data);
  emit self->flashFirmwareProgress(progress);
}

void BaroboBridge::flashFirmwareCompletionEvent (int complete, void* user_data) {
  auto self = static_cast<BaroboBridge*>(user_data);
  emit self->flashFirmwareCompletion(complete);
}
#endif

QLinkbot* BaroboBridge::getRobot(QString id)
{
  if(!linkbots_.contains(id)) {
    return NULL;
  } else {
    return linkbots_[id];
  }
}

/* SLOTS */

void BaroboBridge::robotButtonEvent(QLinkbot *l, int button, int event)
{
  emit buttonChanged(l->getSerialID(), button, event);
}

void BaroboBridge::robotJointsEvent(QLinkbot *l, double j1, double j2, double j3, int mask)
{
  emit motorsChanged(l->getSerialID(), j1, j2, j3, mask);
}

void BaroboBridge::robotJointEvent(QLinkbot *l, int wheel, double angle)
{
  emit motorChanged(l->getSerialID(), wheel, angle);
}

void BaroboBridge::robotAccelEvent(QLinkbot *l, double x, double y, double z)
{
  emit accelChanged(l->getSerialID(), x, y, z);
}

/* PRIVATE SLOTS */

void BaroboBridge::forwardFetchFinished(QUrl url, bool success)
{
  emit fetchFinished(url.toString(), success);
}


FetchTransaction::FetchTransaction(QUrl url, QString dest)
{
  netmanager_ = new QNetworkAccessManager();
  url_ = url;
  dest_ = dest;
}

FetchTransaction::~FetchTransaction()
{
  delete netmanager_;
}

void FetchTransaction::start()
{
  QObject::connect(netmanager_, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(replyFinished(QNetworkReply*)));
  netmanager_->get(QNetworkRequest(url_));
}

void FetchTransaction::replyFinished(QNetworkReply* reply)
{
  if(reply->error()) {
    emit fetchFinished(url_, false);
  } else {
    /* try to save the returned data to the specified file */
    QFile file(dest_);
    if(!file.open(QIODevice::WriteOnly)) {
      emit fetchFinished(url_, false);
      return;
    }
    file.write(reply->readAll());
    qDebug() << reply->readAll();
    file.close();
    emit fetchFinished(url_, true);
  }
}
