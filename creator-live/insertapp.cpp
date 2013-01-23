#include "insertapp.h"
#include <QObject>
#include <Qt>
#include <QScreen>
#include <QQmlContext>

InsertApp::InsertApp()
{
    setTitle("Creator Live");
    rootContext()->setContextProperty("selector", this);
    setResizeMode(SizeRootObjectToView);
    setSource(QUrl("insertApp.qml"));
    setFlags(Qt::Popup);
    setGeometry(0, 50, qGuiApp->primaryScreen()->geometry().width(), qGuiApp->primaryScreen()->QScreen::geometry().height() - 100 );
    show();
}
