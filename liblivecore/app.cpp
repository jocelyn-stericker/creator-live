/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/app>
#include <live/appinterface>

namespace live {

app* app::m_singleton = 0;
Bound<int> app::b_mode(0);

void app::registerInterface(AppInterface* c) {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new app;
    if (m_singleton->m_appNames.contains(c->name())) {
        return;
    }
    m_singleton->m_appNames.push_back(c->name());
    m_singleton->m_apps.push_back(c);
}

app::~app() {
    lthread::assertUi();
    while (m_apps.size()) {
        delete m_apps.takeLast();
    }
    m_singleton = 0;
}

QStringList app::appNames() {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new app;
    return m_singleton->m_appNames;
}

QList<AppInterface*> app::interfaces() {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new app;
    return m_singleton->m_apps;
}

ObjectPtr app::newBackend(QString name) {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new app;
    Q_ASSERT(m_singleton->m_appNames.contains(name));
    ObjectPtr x = (m_singleton
                   ->m_apps[m_singleton->m_appNames.indexOf(name)])
                   ->newBackend();
    x->setTemporary(0);
    return x;
}

QObject* app::newFrontend(QString name, ObjectPtr backend) {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new app;
    Q_ASSERT(m_singleton->m_appNames.contains(name));
    return (m_singleton
            ->m_apps[m_singleton->m_appNames.indexOf(name)])
            ->newFrontend(backend);
}

}
