#include "barobo/qlinkbot.hpp"
#include "qlinkbotworker.hpp"

#include <QMutex>
#include <QWaitCondition>

// baromesh uses Boost preprocessor macros for the time being
#ifndef Q_MOC_RUN
#include "baromesh/baromesh.hpp"
#endif

#include <QThread>
#include <QDebug>

#include <iostream>

inline QDebug operator<< (QDebug dbg, const rpc::VersionTriplet& triplet) {
    QDebugStateSaver s { dbg };
    dbg.nospace() << triplet.major() << '.' << triplet.minor() << '.' << triplet.patch();
    return dbg;
}

using MethodIn = rpc::MethodIn<barobo::Robot>;

struct QLinkbot::Impl {
    Impl (const QString& id, QLinkbot* parent)
        : serialId(id)
        , worker(parent)
        , proxy(id.toStdString()) { }

    QString serialId;
    QMutex lock;
    QWaitCondition cond;
    QThread workerthread;
    QLinkbotWorker worker;
    robot::Proxy proxy;
};

QLinkbot::QLinkbot(const QString& id)
        : m(new QLinkbot::Impl(id, this))
{
    m->worker.moveToThread(&m->workerthread);
    m->workerthread.start();
    // TODO worker object should go away, wire these up from robot proxy
    QObject::connect(&m->worker, SIGNAL(accelChanged(double, double, double)),
        this, SLOT(newAccelValues(double, double, double)),
        Qt::QueuedConnection);
    QObject::connect(&m->worker, SIGNAL(buttonChanged(int, int)),
        this, SLOT(newButtonValues(int, int)));
    QObject::connect(&m->worker, SIGNAL(motorChanged(double, double, double, int)),
        this, SLOT(newMotorValues(double, double, double, int)));
    QMetaObject::invokeMethod(&m->worker, "doWork", Qt::QueuedConnection); 
}

// Needed for unique_ptr, see http://herbsutter.com/gotw/_100/
QLinkbot::~QLinkbot () {
    m->workerthread.quit();
    m->workerthread.wait();
}

void swap (QLinkbot& lhs, QLinkbot& rhs) {
    using std::swap;
    swap(lhs.m, rhs.m);
}

QLinkbot::QLinkbot (QLinkbot&& other) {
    swap(*this, other);
}

void QLinkbot::disconnectRobot()
{
}

void QLinkbot::connectRobot()
{
    auto serviceInfo = m->proxy.connect().get();

    // Check version before we check if the connection succeeded--the user will
    // probably want to know to flash the robot, regardless.
    if (serviceInfo.rpcVersion() != rpc::Version<>::triplet()) {
        throw VersionMismatch(m->serialId.toStdString() + " RPC version " +
            to_string(serviceInfo.rpcVersion()) + " != local RPC version " +
            to_string(rpc::Version<>::triplet()));
    }
    else if (serviceInfo.interfaceVersion() != rpc::Version<barobo::Robot>::triplet()) {
        throw VersionMismatch(m->serialId.toStdString() + " Robot interface version " +
            to_string(serviceInfo.interfaceVersion()) + " != local Robot interface version " +
            to_string(rpc::Version<barobo::Robot>::triplet()));
    }

    if (serviceInfo.connected()) {
        qDebug().nospace() << qPrintable(m->serialId) << ": connected";
    }
    else {
        throw ConnectionRefused(m->serialId.toStdString() + " refused our connection");
    }
}

int QLinkbot::enableAccelEventCallback()
{
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
}

int QLinkbot::enableButtonCallback()
{
    try {
        m->proxy.fire(MethodIn::enableButtonEvent{true}).get();
    }
    catch (...) {
        return -1;
    }
    return 0;
}

int QLinkbot::enableJointEventCallback()
{
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
}

QString QLinkbot::getSerialID() const {
    return m->serialId;
}

void QLinkbot::lock()
{
    m->lock.lock();
}

void QLinkbot::unlock()
{
    m->lock.unlock();
}

void QLinkbot::newAccelValues(double x, double y, double z)
{
    emit accelChanged(this, x, y, z);
}

void QLinkbot::newButtonValues(int button, int buttonDown)
{
    emit buttonChanged(this, button, buttonDown);
}

void QLinkbot::newMotorValues(double j1, double j2, double j3, int mask)
{
    emit jointsChanged(this, j1, j2, j3, mask);
    double angles[3];
    angles[0] = j1;
    angles[1] = j2;
    angles[2] = j3;
    int i;
    for(i = 0; i < 3; i++) {
        if(mask & (1<<i)) {
            emit jointChanged(this, i+1, angles[i]);
        }
    }
}


int QLinkbot::setJointSpeeds (double, double, double) {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::disableAccelEventCallback () {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::disableButtonCallback () {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::disableJointEventCallback () {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::getJointAngles (double& e1, double& e2, double& e3, int) {
    try {
        auto values = m->proxy.fire(MethodIn::getEncoderValues{}).get();
        assert(values.values_count >= 3);
        e1 = values.values[0];
        e2 = values.values[1];
        e3 = values.values[2];
    }
    catch (...) {
        return -1;
    }
    return 0;
}
int QLinkbot::moveNB (double, double, double) {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::moveToNB (double, double, double) {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::setColorRGB (int r, int g, int b) {
    try {
        m->proxy.fire(MethodIn::setLedColor{
            uint32_t(r << 16 | g << 8 | b)
        }).get();
    }
    catch (...) {
        return -1;
    }
    return 0;
}
int QLinkbot::setJointEventThreshold (int, double) {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}
int QLinkbot::stop () {
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}

int QLinkbot::setBuzzerFrequencyOn (float freq) {
    try {
        m->proxy.fire(MethodIn::setBuzzerFrequency{freq}).get();
    }
    catch (...) {
        return -1;
    }
    return 0;
}

int QLinkbot::getVersions (uint32_t& major, uint32_t& minor, uint32_t& patch) {
    try {
        auto version = m->proxy.fire(MethodIn::getFirmwareVersion{}).get();
        major = version.major;
        minor = version.minor;
        patch = version.patch;
        qDebug().nospace() << qPrintable(m->serialId) << " Firmware version "
                           << major << '.' << minor << '.' << patch;
    }
    catch (...) {
        return -1;
    }
    return 0;
}