#include <QtGui/QGuiApplication>
#include <QQuickItem>
#include <QPluginLoader>
#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("app/native/qml/creator-mobile/mobile.qml"));
    viewer.showExpanded();
    QMetaObject::invokeMethod(viewer.rootObject(), "init");

    return app.exec();
}
