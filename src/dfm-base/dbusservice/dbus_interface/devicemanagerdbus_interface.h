/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DeviceManagerInterface -p dbus_interface/devicemanagerdbus_interface devicemanagerdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DEVICEMANAGERDBUS_INTERFACE_H
#define DEVICEMANAGERDBUS_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.filemanager.service.DeviceManager
 */
class DeviceManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.filemanager.service.DeviceManager"; }

public:
    DeviceManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~DeviceManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> DetachAllMountedDevices()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("DetachAllMountedDevices"), argumentList);
    }

    inline QDBusPendingReply<> DetachAllMountedDevicesForced()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("DetachAllMountedDevicesForced"), argumentList);
    }

    inline QDBusPendingReply<> DetachBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("DetachBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<> DetachBlockDeviceForced(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("DetachBlockDeviceForced"), argumentList);
    }

    inline QDBusPendingReply<> DetachProtocolDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("DetachProtocolDevice"), argumentList);
    }

    inline QDBusPendingReply<> EjectBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("EjectBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetBlockDevicesIdList(const QVariantMap &opts)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(opts);
        return asyncCallWithArgumentList(QStringLiteral("GetBlockDevicesIdList"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetProtocolDevicesIdList()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetProtocolDevicesIdList"), argumentList);
    }

    inline QDBusPendingReply<bool> IsMonotorWorking()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("IsMonotorWorking"), argumentList);
    }

    inline QDBusPendingReply<> LockBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("LockBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<QString> MountBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("MountBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<QString> MountProtocolDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("MountProtocolDevice"), argumentList);
    }

    inline QDBusPendingReply<> PoweroffBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("PoweroffBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> QueryBlockDeviceInfo(const QString &id, bool detail)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id) << QVariant::fromValue(detail);
        return asyncCallWithArgumentList(QStringLiteral("QueryBlockDeviceInfo"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> QueryProtocolDeviceInfo(const QString &id, bool detail)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id) << QVariant::fromValue(detail);
        return asyncCallWithArgumentList(QStringLiteral("QueryProtocolDeviceInfo"), argumentList);
    }

    inline QDBusPendingReply<bool> RenameBlockDevice(const QString &id, const QString &newName)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id) << QVariant::fromValue(newName);
        return asyncCallWithArgumentList(QStringLiteral("RenameBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<> SafelyRemoveBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("SafelyRemoveBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<QString> UnlockBlockDevice(const QString &id, const QString &passwd)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id) << QVariant::fromValue(passwd);
        return asyncCallWithArgumentList(QStringLiteral("UnlockBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<> UnmountBlockDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("UnmountBlockDevice"), argumentList);
    }

    inline QDBusPendingReply<> UnmountBlockDeviceForced(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("UnmountBlockDeviceForced"), argumentList);
    }

    inline QDBusPendingReply<> UnmountProtocolDevice(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("UnmountProtocolDevice"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void AskStopSacnningWhenUnmount(const QString &id);
    void AskStopScanningWhenDetach(const QString &id);
    void AskStopScanningWhenDetachAll();
    void BlockDeviceAdded(const QString &id);
    void BlockDeviceFilesystemAdded(const QString &id);
    void BlockDeviceFilesystemRemoved(const QString &id);
    void BlockDeviceLocked(const QString &id);
    void BlockDeviceMounted(const QString &id, const QString &mountPoint);
    void BlockDevicePropertyChanged(const QString &id, const QString &property, const QDBusVariant &value);
    void BlockDeviceRemoved(const QString &id);
    void BlockDeviceUnlocked(const QString &id, const QString &clearDeviceId);
    void BlockDeviceUnmounted(const QString &id);
    void BlockDriveAdded();
    void BlockDriveRemoved();
    void NotifyDeviceBusy(int action);
    void ProtocolDeviceAdded(const QString &id);
    void ProtocolDeviceMounted(const QString &id, const QString &mountPoint);
    void ProtocolDeviceRemoved(const QString &id);
    void ProtocolDeviceUnmounted(const QString &id);
    void SizeUsedChanged(const QString &id, qlonglong total, qlonglong free);
};

namespace com {
  namespace deepin {
    namespace filemanager {
      namespace service {
        typedef ::DeviceManagerInterface DeviceManager;
      }
    }
  }
}
#endif
