#include "insertapp.h"
#include <QObject>
#include <Qt>
#include <QScreen>
#include <QQmlContext>
#include <QDir>

QList<InsertApp*> InsertApp::m_identity;

InsertApp::InsertApp()
{
    m_identity.push_back(this);
    setTitle("Creator Live");
    rootContext()->setContextProperty("selector", this);
    setResizeMode(SizeRootObjectToView);

    QDir pluginsDir = QDir(qApp->applicationDirPath()+"/../plugins");
    setSource(QUrl(pluginsDir.absolutePath() + "/insertApp.qml"));
    setFlags(Qt::Popup);
    setGeometry(0, 50, qGuiApp->primaryScreen()->geometry().width(), qGuiApp->primaryScreen()->QScreen::geometry().height() - 100 );
    show();
}

InsertApp::~InsertApp() {
    m_identity.removeOne(this);
}
