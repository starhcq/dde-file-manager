/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DBusDock -p dbusdock com.deepin.dde.daemon.Dock.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "dbusdock.h"

/*
 * Implementation of interface class DBusDock
 */
DBusDock::DBusDock(QObject *parent)
    : QDBusAbstractInterface(staticServiceName(), staticObjectPath(), staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
    qDBusRegisterMetaType<DockRect>();
    QDBusConnection::sessionBus().connect(this->service(), this->path(),
            "org.freedesktop.DBus.Properties",  "PropertiesChanged",
            this, SLOT(__propertyChanged__(QDBusMessage)));
}

DBusDock::~DBusDock()
{
    QDBusConnection::sessionBus().disconnect(service(), path(), "org.freedesktop.DBus.Properties"
                     ,  "PropertiesChanged", this, SLOT(__propertyChanged__(QDBusMessage)));
}

QDBusArgument &operator<<(QDBusArgument &argument, const DockRect &rect)
{
    argument.beginStructure();
    argument << rect.x << rect.y << rect.width << rect.height;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DockRect &rect)
{
    argument.beginStructure();
    argument >> rect.x >> rect.y >> rect.width >> rect.height;
    argument.endStructure();
    return argument;
}

QDebug operator<<(QDebug deg, const DockRect &rect)
{
    qDebug() << "x:" << rect.x << "y:" << rect.y << "width:" << rect.width << "height:" << rect.height;

    return deg;
}

DockInfo *DockInfo::ins()
{
    static DockInfo ins;
    return  &ins;
}

DBusDock *DockInfo::dock() const
{
    return m_dock;
}

DockInfo::DockInfo(QObject *parent)
    : QObject (parent)
{
    m_dock = new DBusDock(this);
}