/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp ../../../../../dfm-base/dbusservice/org.deepin.filemanager.tag.xml -i ../tagdbus.h -a tagdbus_adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef TAGDBUS_ADAPTOR_H
#define TAGDBUS_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "../tagdbus.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface local.TagDBus
 */
class TagDBusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.TagDBus")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"local.TagDBus\">\n"
"    <method name=\"Insert\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"value\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"    </method>\n"
"    <method name=\"Insert\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"    </method>\n"
"    <method name=\"Delete\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"value\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"    </method>\n"
"    <method name=\"Delete\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"    </method>\n"
"    <method name=\"Update\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"value\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"    </method>\n"
"    <method name=\"Update\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"y\" name=\"type\"/>\n"
"    </method>\n"
"    <method name=\"CanTagFile\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"filePath\"/>\n"
"    </method>\n"
"    <method name=\"LastError\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    TagDBusAdaptor(QObject *parent);
    virtual ~TagDBusAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool CanTagFile(const QString &filePath);
    bool Delete(uchar type);
    bool Delete(uchar type, const QVariantMap &value);
    bool Insert(uchar type);
    bool Insert(uchar type, const QVariantMap &value);
    QString LastError();
    bool Update(uchar type);
    bool Update(uchar type, const QVariantMap &value);
Q_SIGNALS: // SIGNALS
};

#endif
