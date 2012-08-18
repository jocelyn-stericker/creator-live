/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef ASYNCCONNECT_P_H
#define ASYNCCONNECT_P_H

#include <QObject>
#include <QMetaMethod>
#include "live/object"

namespace live_private {

class LIBLIVECORESHARED_EXPORT ASyncConnection : public QObject
{
    QObject* s_obj;
    const char* s_aSignal;
    QObject* s_dest;
    QString s_dSlot;
public:
    ASyncConnection(QObject *obj, const char *aSignal,QObject* dest, QString dSlot) :
        s_obj(obj), s_aSignal(aSignal), s_dest(dest), s_dSlot(dSlot)
    {
        if(s_dSlot.size()) s_dSlot.remove(0,1);
        if(s_dSlot.contains('(')) s_dSlot.truncate(s_dSlot.indexOf('('));
#ifdef Q_CC_BOR
        const int memberOffset = QObject::staticMetaObject.methodCount();
#else
        static const int memberOffset = QObject::staticMetaObject.methodCount();
#endif
        Q_ASSERT(obj);
        Q_ASSERT(aSignal);

        if (((aSignal[0] - '0') & 0x03) != QSIGNAL_CODE) {
            qWarning("ASyncConnection: Not a valid signal, use the SIGNAL macro");
            return;
        }

        QByteArray ba = QMetaObject::normalizedSignature(aSignal + 1);
        const QMetaObject *mo = obj->metaObject();
        int sigIndex = mo->indexOfMethod(ba.constData());
        if (sigIndex < 0) {
            qWarning("ASyncConnection: No such signal: '%s'", ba.constData());
            return;
        }

        if (!QMetaObject::connect(obj, sigIndex, this, memberOffset,
                                  Qt::DirectConnection, 0)) {
            qWarning("ASyncConnection: QMetaObject::connect returned false. Unable to connect.");
            return;
        }
        sig = ba;
        initArgs(mo->method(sigIndex));
    }

    inline bool isValid() const { return !sig.isEmpty(); }
    inline QByteArray signal() const { return sig; }


    int qt_metacall(QMetaObject::Call call, int methodId, void **a)
    {
        methodId = QObject::qt_metacall(call, methodId, a);
        if (methodId < 0)
            return methodId;

        if (call == QMetaObject::InvokeMetaMethod) {
            QList<QVariant> list;
            for (int i = 0; i < args.count(); ++i) {
                QMetaType::Type type = static_cast<QMetaType::Type>(args.at(i));
                list << QVariant(type, a[i + 1]);
            }

            live_async {
                bool ok=1;
                qDebug() << "SLOT:"<<s_dSlot;
                if(list.size()==4||!ok) {
                    ok=QMetaObject::invokeMethod(s_dest,s_dSlot.toAscii(),Qt::DirectConnection,QGenericArgument(list[0].typeName(),a[1]),QGenericArgument(list[1].typeName(),a[2]),QGenericArgument(list[2].typeName(),a[3]),QGenericArgument(list[3].typeName(),a[4]));
                }
                if(list.size()==3||!ok) {
                    ok=QMetaObject::invokeMethod(s_dest,s_dSlot.toAscii(),Qt::DirectConnection,QGenericArgument(list[0].typeName(),a[1]),QGenericArgument(list[1].typeName(),a[2]),QGenericArgument(list[2].typeName(),a[3]));
                }
                if(list.size()==2||!ok) {
                    ok=QMetaObject::invokeMethod(s_dest,s_dSlot.toAscii(),Qt::DirectConnection,QGenericArgument(list[0].typeName(),a[1]),QGenericArgument(list[1].typeName(),a[2]));
                }
                if(list.size()==1||!ok) {
                    ok=QMetaObject::invokeMethod(s_dest,s_dSlot.toAscii(),Qt::DirectConnection,QGenericArgument(list[0].typeName(),a[1]));
                }
                if(list.size()==0||!ok) {
                    ok=QMetaObject::invokeMethod(s_dest,s_dSlot.toAscii(),Qt::DirectConnection);
                }
            }

            --methodId;
        }
        return methodId;
    }

private:
    void initArgs(const QMetaMethod &member)
    {
        QList<QByteArray> params = member.parameterTypes();
        for (int i = 0; i < params.count(); ++i) {
            int tp = QMetaType::type(params.at(i).constData());
            if (tp == QMetaType::Void)
                qWarning("Don't know how to handle '%s', use qRegisterMetaType to register it.",
                         params.at(i).constData());
            args << tp;
        }
    }

    // the full, normalized signal name
    QByteArray sig;
    // holds the QMetaType types for the argument list of the signal
    QList<int> args;
};

class LIBLIVECORESHARED_EXPORT ASyncConnectSys : public QObject {
    Q_OBJECT
    static ASyncConnectSys* s_singleton;
public:
    static ASyncConnectSys* me() { return s_singleton=(s_singleton?s_singleton:new ASyncConnectSys); }
    bool newConnection(QObject* sender, const char* signal, QObject* receiver, const char* method) {
        new ASyncConnection(sender,signal,receiver,method);
        return 1;
    }
};

}

#endif // ASYNCCONNECT_P_H
