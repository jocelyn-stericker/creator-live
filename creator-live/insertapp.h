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
    static QList<InsertApp*> m_identity;
public:
    explicit InsertApp();
    virtual ~InsertApp();

    Q_INVOKABLE void cancel() { emit canceled(); deleteLater(); }
    Q_INVOKABLE void go(QString s) { emit invoked(s); deleteLater(); }

    static void quit() {
        for (int i = 0; i < m_identity.size(); ++i) m_identity[i]->cancel();
    }

signals:
    void canceled();
    void invoked(QString);
};

#endif // INSERTAPP_H
