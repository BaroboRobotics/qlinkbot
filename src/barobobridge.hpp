#ifndef BAROBOBROWSER_BAROBOBRIDGE_H
#define BAROBOBROWSER_BAROBOBRIDGE_H

#include "barobo/qbarobo_global.h"

#include <QCoreApplication>
#include <QNetworkReply>
#include <QObject>
#include <QVector>
#include <QStringList>

#include "barobo/qlinkbot.hpp"

class BaroboBridge : public QObject
{
    Q_OBJECT

public:
    BaroboBridge();

    Q_INVOKABLE void angularSpeed(QString id, double s1, double s2, double s3);
    Q_INVOKABLE void beginScan(QString id);
    Q_INVOKABLE int connectRobot(QString id);
    Q_INVOKABLE void disableAccelSignals(QString id) {
      enableAccelSignals(id, false);
    }
    Q_INVOKABLE void disableButtonSignals(QString id) {
      enableButtonSignals(id, false);
    }
    Q_INVOKABLE void disableMotorSignals(QString id) {
      enableMotorSignals(id, false);
    }
    Q_INVOKABLE void disconnectRobot(QString id);
    Q_INVOKABLE void enableButtonSignals(QString id, bool enable=true);
    Q_INVOKABLE void enableMotorSignals(QString id, bool enable=true);
    Q_INVOKABLE void enableAccelSignals(QString id, bool enable=true);
    Q_INVOKABLE void fetch(QVariantMap urlInfo);
    Q_INVOKABLE QVariantList getMotorAngles(QString id);
    Q_INVOKABLE void move(QString id, double j1, double j2, double j3);
    Q_INVOKABLE void moveTo(QString id, double j1, double j2, double j3);
    Q_INVOKABLE int numConnectedRobots();
    Q_INVOKABLE void setLEDColor(QString id, int r, int g, int b);
    Q_INVOKABLE void setMotorEventThreshold(QString id, int motor, double degrees);
    Q_INVOKABLE void stop(QString id);

    Q_INVOKABLE void buzzerFrequency (QString id, double freq);

    Q_INVOKABLE QString firmwareVersion (QString id);
    Q_INVOKABLE QStringList availableFirmwareVersions ();
    Q_INVOKABLE bool canFlashFirmware (QString id);
    Q_INVOKABLE int flashFirmware (QString id, QString version);

    /* For testing */
    void addRobot(QLinkbot* linkbot);

public slots:
    Q_INVOKABLE void robotButtonEvent(QLinkbot *l, int button, int event);
    Q_INVOKABLE void robotJointsEvent(QLinkbot *l, double j1, double j2, double j3, int mask);
    Q_INVOKABLE void robotJointEvent(QLinkbot *l, int wheel, double angle);
    Q_INVOKABLE void robotAccelEvent(QLinkbot *l, double x, double y, double z);

private slots:
    void forwardFetchFinished(QUrl url, bool success);

signals:
    void buttonChanged(QString id, int button, int event);
    void motorsChanged(QString id, double j1, double j2, double j3, int mask);
    void motorChanged(QString id, int wheel, double angle);
    void accelChanged(QString id, double x, double y, double z);
    void idScanned(QString newId);
    void fetchFinished(QString filename, bool success);

    void flashFirmwareProgress (double progress);
    void flashFirmwareCompletion (int complete);

private:
    static void flashFirmwareProgressEvent (double progress, void* user_data);
    static void flashFirmwareCompletionEvent (int complete, void* user_data);

    /* FIXME these string literals should probably be configured by CMake */
    static QString firmwareDirectory () {
      return qApp->applicationDirPath() + "/firmware";
    }

    static QString firmwareSuffix () {
      return ".hex";
    }

    void deleteRobot(QString id);
    QLinkbot* getRobot(QString id);
    QMap<QString, QLinkbot*> linkbots_;
};

class FetchTransaction : public QObject
{
  Q_OBJECT
  public:
    FetchTransaction(QUrl url, QString dest);
    ~FetchTransaction();

  public slots:
    void start();
    void replyFinished(QNetworkReply* reply);

  signals:
    void fetchFinished(QUrl url, bool success);

  private:
    QNetworkAccessManager* netmanager_;
    QUrl url_;
    QString dest_;
};

#endif
