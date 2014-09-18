#include "barobo/qlinkbot.hpp"

#include <QMutex>
#include <QWaitCondition>

// baromesh uses Boost preprocessor macros for the time being
#ifndef Q_MOC_RUN
#include "baromesh/baromesh.hpp"
#endif

#include <boost/log/common.hpp>
#include <boost/log/sources/logger.hpp>

#include <QThread>
#include <QDebug>

#include <iostream>

#undef M_PI
#define M_PI 3.14159265358979323846

namespace {

template <class T>
T degToRad (T x) { return T(double(x) * M_PI / 180.0); }

template <class T>
T radToDeg (T x) { return T(double(x) * 180.0 / M_PI); }

inline QDebug operator<< (QDebug dbg, const rpc::VersionTriplet& triplet) {
    QDebugStateSaver s { dbg };
    dbg.nospace() << triplet.major() << '.' << triplet.minor() << '.' << triplet.patch();
    return dbg;
}

} // file namespace

using MethodIn = rpc::MethodIn<barobo::Robot>;

struct QLinkbot::Impl {
    Impl (const QString& id)
        : serialId(id)
        , proxy(id.toStdString()) { }

    boost::log::sources::logger_mt log;

    QString serialId;
    robot::Proxy proxy;
};

QLinkbot::QLinkbot(const QString& id)
        : m(new QLinkbot::Impl(id)) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot");
    m->proxy.buttonEvent.connect(BIND_MEM_CB(&QLinkbot::newButtonValues, this));
    m->proxy.encoderEvent.connect(BIND_MEM_CB(&QLinkbot::newMotorValues, this));
    m->log.add_attribute("SerialId", boost::log::attributes::constant<std::string>(id.toStdString()));
    BOOST_LOG(m->log) << "constructed";
}

// Out-of-line destructor (even if empty) is needed for unique_ptr, see
// http://herbsutter.com/gotw/_100/
QLinkbot::~QLinkbot () {
    BOOST_LOG_NAMED_SCOPE("~QLinkbot");;
    BOOST_LOG(m->log) << "destroyed";
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
    BOOST_LOG_NAMED_SCOPE("QLinkbot::disconnectRobot");;
    try {
        m->proxy.disconnect().get();
        BOOST_LOG(m->log) << "disconnected";
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
    }
}

void QLinkbot::connectRobot()
{
    BOOST_LOG_NAMED_SCOPE("QLinkbot::connectRobot");
    auto f = m->proxy.connect();
    BOOST_LOG(m->log) << "sent connection request";

    auto serviceInfo = f.get();

    BOOST_LOG(m->log) << "RPC version " << serviceInfo.rpcVersion();
    BOOST_LOG(m->log) << "interface version " << serviceInfo.interfaceVersion();

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
        BOOST_LOG(m->log) << "connected";
    }
    else {
        throw ConnectionRefused(m->serialId.toStdString() + " refused our connection");
    }
}

int QLinkbot::enableAccelEventCallback()
{
    BOOST_LOG_NAMED_SCOPE("QLinkbot::enableAccelEventCallback");
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
}

int QLinkbot::enableButtonCallback()
{
    BOOST_LOG_NAMED_SCOPE("QLinkbot::enableButtonCallback");
    try {
        m->proxy.fire(MethodIn::enableButtonEvent{true}).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::enableJointEventCallback()
{
    BOOST_LOG_NAMED_SCOPE("QLinkbot::enableJointEventCallback");
    try {
        m->proxy.fire(MethodIn::enableEncoderEvent {
            true, { true, degToRad(float(20.0)) },
            true, { true, degToRad(float(20.0)) },
            true, { true, degToRad(float(20.0)) }
        }).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
    }
}

QString QLinkbot::getSerialID() const {
    return m->serialId;
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

int QLinkbot::setJointSpeeds (double s0, double s1, double s2) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::setJointSpeeds");
    try {
        auto f0 = m->proxy.fire(MethodIn::setMotorControllerOmega { 0, float(degToRad(s0)) });
        auto f1 = m->proxy.fire(MethodIn::setMotorControllerOmega { 1, float(degToRad(s1)) });
        auto f2 = m->proxy.fire(MethodIn::setMotorControllerOmega { 2, float(degToRad(s2)) });
        f0.get();
        f1.get();
        f2.get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::disableAccelEventCallback () {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::disableAccelEventCallback");
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}

int QLinkbot::disableButtonCallback () {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::disableButtonCallback");
    try {
        m->proxy.fire(MethodIn::enableButtonEvent{false}).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::disableJointEventCallback () {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::disableJointEventCallback");
    try {
        m->proxy.fire(MethodIn::enableEncoderEvent {
            true, { false, 0 },
            true, { false, 0 },
            true, { false, 0 }
        }).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
    }
    return 0;
}

int QLinkbot::getJointAngles (double& a0, double& a1, double& a2, int) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::getJointAngles");
    try {
        auto values = m->proxy.fire(MethodIn::getEncoderValues{}).get();
        assert(values.values_count >= 3);
        a0 = radToDeg(values.values[0]);
        a1 = radToDeg(values.values[1]);
        a2 = radToDeg(values.values[2]);
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::moveNB (double a0, double a1, double a2) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::moveNB");
    try {
        m->proxy.fire(MethodIn::move {
            true, { barobo_Robot_Goal_Type_RELATIVE, float(degToRad(a0)) },
            true, { barobo_Robot_Goal_Type_RELATIVE, float(degToRad(a1)) },
            true, { barobo_Robot_Goal_Type_RELATIVE, float(degToRad(a2)) }
        }).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::moveToNB (double a0, double a1, double a2) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::moveToNB");
    try {
        m->proxy.fire(MethodIn::move {
            true, { barobo_Robot_Goal_Type_ABSOLUTE, float(degToRad(a0)) },
            true, { barobo_Robot_Goal_Type_ABSOLUTE, float(degToRad(a1)) },
            true, { barobo_Robot_Goal_Type_ABSOLUTE, float(degToRad(a2)) }
        }).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::stop () {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::stop");
    try {
        m->proxy.fire(MethodIn::stop{}).get();
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::setColorRGB (int r, int g, int b) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::setColorRGB");
    try {
        m->proxy.fire(MethodIn::setLedColor{
            uint32_t(r << 16 | g << 8 | b)
        }).get();
        BOOST_LOG(m->log) << "set color to " << r << ' ' << g << ' ' << b;
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::setJointEventThreshold (int, double) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::setJointEventThreshold");
#warning Unimplemented stub function in qlinkbot
    qWarning() << "Unimplemented stub function in qlinkbot";
    return 0;
}

int QLinkbot::setBuzzerFrequencyOn (float freq) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::setBuzzerFrequencyOn");
    try {
        m->proxy.fire(MethodIn::setBuzzerFrequency{freq}).get();
        BOOST_LOG(m->log) << "set frequency to " << freq;
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}

int QLinkbot::getVersions (uint32_t& major, uint32_t& minor, uint32_t& patch) {
    BOOST_LOG_NAMED_SCOPE("QLinkbot::getVersions");
    try {
        auto version = m->proxy.fire(MethodIn::getFirmwareVersion{}).get();
        major = version.major;
        minor = version.minor;
        patch = version.patch;
        BOOST_LOG(m->log) << "firmware version " << major << '.' << minor << '.' << patch;
    }
    catch (std::exception& e) {
        qDebug().nospace() << qPrintable(m->serialId) << ": " << e.what();
        return -1;
    }
    return 0;
}
