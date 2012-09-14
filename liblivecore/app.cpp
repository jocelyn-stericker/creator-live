/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/app>
#include <live/appinterface>

namespace live {

app* app::s_singleton = 0;

void app::registerInterface(AppInterface* c) {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new app;
    if (s_singleton->s_appNames.contains(c->name())) {
        return;
    }
    s_singleton->s_appNames.push_back(c->name());
    s_singleton->s_apps.push_back(c);
}

app::~app() {
    lthread::assertUi();
    while (s_apps.size()) {
        delete s_apps.takeLast();
    }
    s_singleton = 0;
}

QStringList app::appNames() {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new app;
    return s_singleton->s_appNames;
}

QList<AppInterface*> app::interfaces() {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new app;
    return s_singleton->s_apps;
}

ObjectPtr app::newBackend(QString name) {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new app;
    Q_ASSERT(s_singleton->s_appNames.contains(name));
    ObjectPtr x = (s_singleton
                   ->s_apps[s_singleton->s_appNames.indexOf(name)])
                   ->newBackend();
    x->setTemporary(0);
    return x;
}

QObject* app::newFrontend(QString name, ObjectPtr backend) {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new app;
    Q_ASSERT(s_singleton->s_appNames.contains(name));
    return (s_singleton
            ->s_apps[s_singleton->s_appNames.indexOf(name)])
            ->newFrontend(backend);
}
}
