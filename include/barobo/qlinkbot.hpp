#ifndef QLINKBOT_H__
#define QLINKBOT_H__

#include "qbarobo_global.h"

#include <QObject>

#include <stdexcept>
#include <memory>

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

    // hlh: are these used?
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

    struct ConnectionRefused : std::runtime_error {
        ConnectionRefused (std::string s) : std::runtime_error(s) { }
    };
    struct VersionMismatch : std::runtime_error {
        VersionMismatch (std::string s) : std::runtime_error(s) { }
    };

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

#endif
