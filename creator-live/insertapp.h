#ifndef INSERTAPP_H
#define INSERTAPP_H

#include <QQuickView>
#include <QTimer>
#include "windowwidget.h"
#include <QGuiApplication>
#include <QMouseEvent>

class InsertApp : public QQuickView
{
    Q_OBJECT
    static QList<InsertApp*> s_identity;
public:
    explicit InsertApp();
    virtual ~InsertApp();

    Q_INVOKABLE void cancel() { emit canceled(); deleteLater(); }
    Q_INVOKABLE void go(QString s) { emit invoked(s); deleteLater(); }

    static void quit() {
        while (s_identity.size()) s_identity.takeLast()->deleteLater();
    }

signals:
    void canceled();
    void invoked(QString);
};

#endif // INSERTAPP_H
